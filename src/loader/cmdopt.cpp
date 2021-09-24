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

        if (argc == 0) {
            throw std::logic_error("bad cmd line");
        }

        exe_ = fs::absolute(argv[0]);

        std::vector<std::string> posArgs;
        po::options_description_easy_init add = desc_.add_options();
        addOptions(add);
        add("file,f", po::value<strvec>(&posArgs), "Path(s) of text file to open");
        add("stylefile,s", po::value<std::string>(&stylefile_), "Path of style sheet file");

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
        const std::string tab = "    ";
        const std::string app = exe_.filename().generic_string();
        out << "Usage:" << std::endl;
        out << tab << app << " [Options] file(s)" << std::endl;
        out << std::endl;
        out << "Examples:" << std::endl;
        out << tab << "> " << app << " a.txt" << std::endl;
        out << tab << tab << "open 'a.txt'" << std::endl;
        out << tab << "> " << app << " a.txt b.txt" << std::endl;
        out << tab << tab << "open 'a.txt' and 'b.txt'" << std::endl;
        out << tab << "> " << app << " a.txt 100 b.txt" << std::endl;
        out << tab << tab << "open 'a.txt' at line 100, open 'b.txt' at default line" << std::endl;
        out << std::endl;
        out << desc_ << std::endl;
        out << std::endl;
    }

    const std::vector<std::pair<std::filesystem::path, int64_t>> &files() const {
        return files_;
    }

    const char *logLevel() const {
        if (varmap_.count("logverbose") != 0) {
            return "a";
        }
        if (varmap_.count("logdebug") != 0) {
            return "d";
        }
        if (varmap_.count("logerror") != 0) {
            return "e";
        }
        if (varmap_.count("loginfo") != 0) {
            return "i";
        }
        return "";
    }

    bool shouldLogToStdout() const {
        return varmap_.count("stdout") != 0;
    }

    bool hasNonSingle() const {
        return varmap_.count("nonsingleton") != 0;
    }

    const std::string &stylefile() const {
        return stylefile_;
    }

private:
    void addOptions(boost::program_options::options_description_easy_init &add) {
        add("help,h", "Show this message");
        add("stdout,o", "Log will be also written to stdout");
        add("loginfo,i", "Record logs for important information");
        add("logerror,e", "Record logs for error message");
        add("logdebug,d", "Record logs for debug information");
        add("logverbose,v", "Record all logs");
        add("nonsingleton,S", "Do not start program in singleton mode");
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
                        files_.push_back({ *lastPath, 0 });
                    } else {
                        for (RowN row : rows) {
                            files_.push_back({ *lastPath, row });
                        }
                    }
                }
                lastPath = std::move(path);
                rows.clear();
            } else if (isNum(arg)) {
                // 命令行参数传入的段落数的下标从1开始的，而程序内部使用的下标从0开始
                rows.insert(static_cast<RowN>(std::stoi(arg)) - 1);
            } else {
                // do nothing
            }
        }

        if (lastPath) {
            if (rows.empty()) {
                files_.push_back({ *lastPath, 0 });
            } else {
                for (RowN row : rows) {
                    files_.push_back({ *lastPath, row });
                }
            }
        }
    }

private:
    boost::program_options::options_description desc_{ "Options" };
    boost::program_options::positional_options_description posdesc_;
    fs::path exe_;
    boost::program_options::variables_map varmap_;
    bool hasError_ = false;
    std::vector<std::pair<std::filesystem::path, int64_t>> files_;
    std::string stylefile_;
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

const std::vector<std::pair<std::filesystem::path, int64_t>> &CmdOpt::files() const
{
    return impl_->files();
}

const char *CmdOpt::logLevel() const
{
    return impl_->logLevel();
}

bool CmdOpt::shouldLogToStdout() const
{
    return impl_->shouldLogToStdout();
}

bool CmdOpt::hasNonSingle() const
{
    return impl_->hasNonSingle();
}

std::optional<fs::path> CmdOpt::styleSheetFile() const
{
    std::string stylefile = impl_->stylefile();
    if (stylefile.empty()) {
        return std::nullopt;
    } else {
        return fs::path(stylefile);
    }
}
