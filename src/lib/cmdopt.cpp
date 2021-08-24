#include "cmdopt.h"

#include <string>
#include <vector>
#include <ostream>
#include <optional>

#include <boost/program_options.hpp>

namespace
{
namespace po = boost::program_options;
}

class CmdOptImpl {
public:
    CmdOptImpl(int argc, char *argv[]) {
        using strvec = std::vector<std::string>;

        std::vector<std::string> posArgs;
        po::options_description_easy_init add = desc_.add_options();
        addOptions(add);
        add("file,f", po::value<strvec>(&posArgs), "Path(s) of text file to open");

        po::positional_options_description posdesc_;
        posdesc_.add("file", -1);

        try {
            po::store(po::command_line_parser(argc, argv).options(desc_).positional(posdesc_).run(), varmap_);
            po::notify(varmap_);
        }
        catch (const std::exception &) {
            hasError_ = true;
            return;
        }

        parsePosArgs(posArgs);
    }

    bool needHelp() const {
        return hasError_ || varmap_.count("help") != 0;
    }

    void help(std::ostream &out) const {
        out << "Help:" << std::endl;
        out << desc_ << std::endl;
    }

    const std::vector<doc::OpenInfo> &files() const {
        return files_;
    }

    const std::string &logLevel() const {
        return logLevel_;
    }

    bool shouldLogToStdout() const {
        return varmap_.count("stdout") != 0;
    }

private:
    void addOptions(boost::program_options::options_description_easy_init &add) {
        add("help,h", "Show this message");
        add("loglevel", po::value<std::string>(&logLevel_), "Set Log Level");
        add("stdout", "Log will be also written to stdout");
    }

    static bool isNum(const std::string &s) {
        if (s.empty()) {
            return false;
        }
        for (char c : s) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        return true;
    }

    void parsePosArgs(const std::vector<std::string> &args) {
        std::optional<fs::path> lastPath;
        std::set<RowN> rows;

        for (const std::string &arg : args) {
            fs::path path = arg;
            if (fs::exists(path)) {
                if (lastPath) {
                    if (rows.empty()) {
                        files_.push_back(doc::OpenInfo(*lastPath));
                    } else {
                        for (RowN row : rows) {
                            files_.push_back(doc::OpenInfo(*lastPath, row));
                        }
                    }
                }
                lastPath = std::move(path);
                rows.clear();
            } else if (isNum(arg)) {
                rows.insert(static_cast<RowN>(std::stoi(arg)));
            } else {
                // do nothing
            }
        }

        if (lastPath) {
            if (rows.empty()) {
                files_.push_back(doc::OpenInfo(*lastPath));
            } else {
                for (RowN row : rows) {
                    files_.push_back(doc::OpenInfo(*lastPath, row));
                }
            }
        }
    }

private:
    boost::program_options::options_description desc_{ "Options" };
    boost::program_options::positional_options_description posdesc_;
    boost::program_options::variables_map varmap_;
    bool hasError_ = false;
    std::vector<doc::OpenInfo> files_;
    std::string logLevel_;
};

CmdOpt::CmdOpt(int argc, char *argv[])
    : impl_(new CmdOptImpl(argc, argv))
{
}

CmdOpt::~CmdOpt()
{
    delete impl_;
    impl_ = nullptr;
}

bool CmdOpt::needHelp() const
{
    return impl_->needHelp();
}

void CmdOpt::help(std::ostream &out) const
{
    impl_->help(out);
}

const std::vector<doc::OpenInfo> &CmdOpt::files() const
{
    return impl_->files();
}

const std::string CmdOpt::logLevel() const
{
    return impl_->logLevel();
}

bool CmdOpt::shouldLogToStdout() const
{
    return impl_->shouldLogToStdout();
}
