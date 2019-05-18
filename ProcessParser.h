#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"


using namespace std;

class ProcessParser{
private:
    std::ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:
static string ProcessParser::getCmd(string pid){
    string cmd = "";
    stream = Util::getStream(Path::basePath() + pid + Path::cmdPath());
    getline(stream, cmd);
    return cmd;
}
/**
 * returns a string holding the value of the size of the vm
 */
static std::string getVmSize(string pid){
    string line =  "";
    string name = "VmData";
    float result = 0.0f;
    //We get the status path from the pid path
    stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    
    while(getline(stream, line)){
        if(line.fine(name) != npos){
            istringstream buf(line);
            //We need a input stream iterator to acess the values in vm
            istream_iterator value_stream beg(buf), end;
            vector<string> values(beg, end);
            result = (stof(values[1])/float(1024));
            break;
        }
    }
    return to_string(result);
}
std::string ProcessParser::getCpuPercent(){
    string line;
    float result;
    stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    getline(stream, line);
    istringstream buf (line);
    istream_iterator value_stream beg(buf), end;
    vector<string> values(beg, end);
    float uptime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float start_time = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = uptime + stime + cutime + cstime;
    float second = uptime - (start_time/freq);
    result = 100*((total_time/freq)/seconds);
    return to_string(result);
}

static std::string ProcessParser::getProcUpTime(string pid){
    string value;
    string line;
    stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    getline(stream, line);
    istringstream buf (line);
    istream_iterator value_stream beg(buf), end;
    vector<string> values(beg, end);
    return to_string(float(stof(values[13]))/sysconf(_SC_CLK_TCK));
}

static long int ProcessParser::getSysUpTime(){
    string line;
    stream = Util::getStream(Path::basePath() + Path::upTimePath());
    getline(stream, line);

    istringstream buf (line);
    istream_iterator value_stream beg(buf), end;
    vector<string> values(beg, end);

    return stoi(values[0]);
}

static string ProcessParser::getProcUser(string pid){
    string line =  "";
    string name = "Uid";
    string result = "";
    //We get the status path from the pid path
    stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    
    while(getline(stream, line)){
        if(line.fine(name) != std::string::npos){
            istringstream buf(line);
            //We need a input stream iterator to acess the values in vm
            istream_iterator value_stream beg(buf), end;
            vector<string> values(beg, end);
            result = values[1];
            break;
        }
    }

    name = "x:" + result;
    stream = Util::getStream(/etc/passwd-);
    
    while(getline(stream, line)){
        if(line.fine(name) != std::string::npos){
            return result;
            break;
        }
    }
}
bool allDigit(strig str){
    for(auto i : str){
        if(!(isdigit(i))){
            return false;
        }
    }
}

static vector<string> ProcessParser::getPidList(){
    DIR* dir;
    vector<string>container;
    if(!(dir.open("/proc")))
        throw runtime_error(error(error));
    
    while(dirent* dirp = readdir(dir)){
        if(dirp->d_type != DT_DIR){
            continue;
        }
        if(allDigit(dirp->name)){
            container.push_back(dirp->name);
        }
    }

    if(!closedir(dir))
        throw runtime_error(error(error));
    return container;
}