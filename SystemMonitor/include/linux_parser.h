#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <cmath>
#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// process file extraction
/* ParseProcessFile extracts required information from process information
 * pseudo-filesystem in Linux. By providing the path to the file, either the
 * lookup token or the row number, delimiter and columns the required
 * information are read from file and returned as a vector. You can only provide
 * either the lookup token or the row number, by providing both, the lookup
 * token has higher priority. If you want to parse information from a certain
 * row, then you have to invoke the function with an empty string for lookup
 * token.
 *
 * see https://man7.org/linux/man-pages/man5/proc.5.html
 *
 * @param abs_path: Absolute path to the process file from which the required
 * information is to be read
 *
 * @param lookup_token: A string identifying the first word in the process
 * information file's line from which the required information has to be parsed.
 * If this parameter is not empty, then row is ignored. In case lookup_token is
 * empty, then row is considered.
 *
 * @param delimiter: String identifying the separation between the individual
 * columns in the process information file's line.
 *
 * @param row: identifies the number of the process information file's line from
 * which the required information has to be parsed. If lookup_token is not
 * empty, then the "row" is ignored. In case "lookup_token" is empty, then "row"
 * is considered. row is zero based, that is row=0 corresponds to the 1st line,
 * row=1 to the 2nd line etc.
 *
 * @param columns: Collection of columns in a line from which the required
 * information has to be parsed. "column" is zero based, that is "column"=0
 * corresponds to the 1st column, "column"=1 to the 2nd column etc.
 *
 * @return: Collection of parsed information which has the same size as the
 * parameter "columns".
 */

std::vector<std::string> ParseProcessFile(const std::string& abs_path,
                                          const std::string& lookup_token,
                                          char delimiter, int row,
                                          std::vector<int>& columns);

// text line extraction
/* ParseLine extracts required information from a text line by providing the
 * line from which the information are to be extracted and the columns
 * corresponding to the words in the text line.
 *
 * @param line: line from which the information are to be extracted.
 *
 * @param columns: Collection of columns in a line from which the required
 * information has to be parsed. "column" is zero based, that is "column"=0
 * corresponds to the 1st column, "column"=1 to the 2nd column etc.
 *
 * @return: Collection of parsed information which has the same size as the
 * parameter "columns".
 */
std::vector<std::string> ParseLine(const std::string& line,
                                   std::vector<int>& columns);

/*
 * Compare two floating point numbers against equality
 * @see https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
 */
template <class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y, int ulp) {
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  return std::fabs(x - y) <=
             std::numeric_limits<T>::epsilon() * std::fabs(x + y) * ulp
         // unless the result is subnormal
         || std::fabs(x - y) < std::numeric_limits<T>::min();
}

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum class CPU_STATES : unsigned int {
  user = 0,
  nice,
  system,
  idle,
  io_wait,
  irq,
  soft_irq,
  steal,
  guest,
  guest_nice
};

std::vector<std::string> CpuTimeParser();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
enum class PROCESS_STAT : unsigned int {
  state = 0,
  utime,
  stime,
  cutime,
  cstime
};

std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
std::vector<std::string> ProcessStatus(int pid);
long int UpTime(int pid);
char ProcessState(int pid);
};  // namespace LinuxParser

#endif