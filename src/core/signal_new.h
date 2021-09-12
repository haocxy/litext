#pragma once

#include <map>
#include <memory>
#include <vector>
#include <functional>
#include <shared_mutex>

namespace signal
{

namespace detail
{


struct NonTemplateConnControllBlock {

    virtual ~NonTemplateConnControllBlock() {}

    virtual void disconnect() = 0;
};



template <typename F>
struct SignalControllBlock : public std::enable_shared_from_this<SignalControllBlock<F>> {
public:

    struct ConnectionControllBlock : public NonTemplateConnControllBlock {
    public:
        ConnectionControllBlock(std::function<F> &&callback, std::weak_ptr<detail::SignalControllBlock<F>> sigControllBlock)
            : callback_(std::move(callback)), signalControllBlock_(sigControllBlock) {}

        virtual ~ConnectionControllBlock() {
            disconnect();
        }

        virtual void disconnect() override {
            Lock lock(mtx_);
            if (!signalControllBlock_.expired()) {
                callback_ = nullptr;
                std::shared_ptr<detail::SignalControllBlock<F>> signal = signalControllBlock_.lock();
                signal->disconnect(this);
            }
        }

        template <typename ...Args>
        void call(Args &&...args) {
            Mtx mtx_;
            if (callback_) {
                callback_(std::forward<Args>(args)...);
            }
        }

    private:
        using Mtx = std::mutex;
        using Lock = std::lock_guard<Mtx>;
        Mtx mtx_;
        std::function<F> callback_;
        std::weak_ptr<detail::SignalControllBlock<F>> signalControllBlock_;
    };

    SignalControllBlock() {}

    SignalControllBlock(const SignalControllBlock &) = delete;
    SignalControllBlock(SignalControllBlock &&) = delete;
    SignalControllBlock &operator=(const SignalControllBlock &) = delete;
    SignalControllBlock &operator=(SignalControllBlock &&) = delete;

    virtual ~SignalControllBlock() {}

    std::weak_ptr<ConnectionControllBlock> connect(std::function<F> &&cb)
    {
        WriteLock lock(mtx_);
        auto connControllBlock = std::make_shared<ConnectionControllBlock>(std::move(cb), shared_from_this());
        conns_.push_back(connControllBlock);
        return connControllBlock;
    }

    void disconnect(ConnectionControllBlock *id) {
        WriteLock lock(mtx_);
        auto it = conns_.begin();
        while (it != conns_.end()) {
            if (it->get() == id) {
                it = conns_.erase(it);
            } else {
                ++it;
            }
        }
    }

    template <typename ...Args>
    void call(Args &&...args) {
        ReadLock lock(mtx_);
        for (auto &p : conns_) {
            if (p) {
                p->call(std::forward<Args>(args)...);
            }
        }
    }

private:
    using Mtx = std::shared_mutex;
    using ReadLock = std::shared_lock<Mtx>;
    using WriteLock = std::lock_guard<Mtx>;
    mutable Mtx mtx_;
    std::list<std::shared_ptr<ConnectionControllBlock>> conns_;
};

}


class Connection {
public:
    Connection() {}

    Connection(std::weak_ptr<detail::NonTemplateConnControllBlock> connControllBlock)
        :connControllBlock_(connControllBlock) {}

    Connection(const Connection &) = delete;

    Connection(Connection &&b) noexcept
        : connControllBlock_(std::move(b.connControllBlock_)){}

    Connection &operator=(const Connection &) = delete;

    Connection &operator=(Connection &&b) noexcept {
        connControllBlock_ = std::move(b.connControllBlock_);
        return *this;
    }

    ~Connection() {
        disconnect();
    }

    void disconnect()
    {
        if (!connControllBlock_.expired()) {
            std::shared_ptr<detail::NonTemplateConnControllBlock> conn = connControllBlock_.lock();
            conn->disconnect();
        }
    }

private:
    std::weak_ptr<detail::NonTemplateConnControllBlock> connControllBlock_;
};

template <typename F>
class Signal {
public:
    Signal() : sigControllBlock_(std::make_shared<detail::SignalControllBlock<F>>()) {}

    Connection connect(std::function<F> &&cb) {
        return sigControllBlock_->connect(std::move(cb));
    }

    template <typename ...Args>
    void operator()(Args &&...args) {
        if (sigControllBlock_) {
            sigControllBlock_->call(std::forward<Args>(args)...);
        }
    }

private:
    std::shared_ptr<detail::SignalControllBlock<F>> sigControllBlock_;
};

}
