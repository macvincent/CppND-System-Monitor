#include "ProcessParser.h"

// TODO: Define all of the above functions below:
string ProcessParser::getCmd(string pid){
    string cmd = "";
    stream = Util::getStream(Path::basePath() + pid + Path::cmdPath());
    getline(stream, cmd);
    return cmd;
}
/**
 * returns a string holding the value of the size of the vm
 */
string ProcessParser::getVmSize(string pid){
    string line =  "";
    string name = "VmData";
    float result = 0.0f;
    //We get the status path from the pid path
    stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            istringstream buf(line);
            //We need a input stream iterator to acess the values in vm
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result = (stof(values[1])/float(1024));
            break;
        }
    }
    return to_string(result);
}
string ProcessParser::getCpuPercent(string pid){
    string line;
    float result;
    stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    getline(stream, line);
    istringstream buf (line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    float uptime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float start_time = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = uptime + stime + cutime + cstime;
    float seconds = uptime - (start_time/freq);
    result = 100*((total_time/freq)/seconds);
    return to_string(result);
}

std::string ProcessParser::getProcUpTime(string pid){
    string value;
    string line;
    stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    getline(stream, line);
    istringstream buf (line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    return to_string(float(stof(values[13]))/sysconf(_SC_CLK_TCK));
}

long int ProcessParser::getSysUpTime(){
    string line;
    stream = Util::getStream(Path::basePath() + Path::upTimePath());
    getline(stream, line);

    istringstream buf (line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);

    return stoi(values[0]);
}

string ProcessParser::getProcUser(string pid){
    string line =  "";
    string name = "Uid";
    string result = "";
    //We get the status path from the pid path
    stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    
    while(getline(stream, line)){
        if(line.find(name) != std::string::npos){
            istringstream buf(line);
            //We need a input stream iterator to acess the values in vm
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result = values[1];
            break;
        }
    }

    name = "x:" + result;
    stream = Util::getStream("/etc/passwd-");
    
    while(getline(stream, line)){
        if(line.find(name) != std::string::npos){
            return result;
            break;
        }
    }
}
bool allDigit(string str){
    for(auto i : str){
        if(!(isdigit(i))){
            return false;
        }
    }
}

vector<string> ProcessParser::getPidList(){
    DIR* dir;
    vector<string>container;
    if(!(dir = opendir("/proc")))
        throw runtime_error(strerror(errno));
    
    while(dirent* dirp = readdir(dir)){
        if(dirp->d_type != DT_DIR){
            continue;
        }
        if(allDigit(dirp->d_name)){
            container.push_back(dirp->d_name);
        }
    }

    if(!closedir(dir))
        throw runtime_error(strerror(errno));
    return container;
}

int ProcessParser::getNumberOfCpuCores(){
    int result = 0;
    string line = "";
    string name = "cpu cores";
    stream = Util::getStream(Path::basePath() + "cpuinfo");
    while(getline(stream, line)){
        if(line.find(name) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            result = stoi(values[2]);
        }
    }
    return result;
}
vector<string> ProcessParser::getSysCpuPercent(string coreNumber = ""){
    string line = "";
    coreNumber = to_string(getNumberOfCpuCores());
    stream = Util::getStream(Path::basePath() + Path::statPath());
    while(getline(stream, line)){
        if(line.find(coreNumber) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            return values;
        }
    }
    return (vector<string>());
}

float get_sys_active_cpu_time(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float get_sys_idle_cpu_time(vector<string>values)
{
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string> values2){
    /**
     * CPU stats are time-dependent
     * So the only way to get valid CPU statistics are by specifying a time interval
     */
    float active_time = get_sys_active_cpu_time(values2)-get_sys_active_cpu_time(values1);
    float idle_time = get_sys_idle_cpu_time(values2) - get_sys_idle_cpu_time(values1);
    float total_time = active_time + idle_time;
    float result = 100.0*(active_time / total_time);
    return to_string(result);
}

float ProcessParser::getSysRamPercent(){
    float sysRamPercent = 0.0f;
    string line = "";
    stream = Util::getStream(Path::basePath() + Path::memInfoPath());
    string name1 = "MemAvailable:";
    string name2 = "MemFree:";
    string name3 = "Buffers:";

    float totalMem = 0.0f;
    float freeMem = 0.0f;
    float buffers = 0.0f;

    while(getline(stream, line)){
        if(line.find(name1) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            totalMem = stof(values[1]);
        }
        if(line.find(name2) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            freeMem = stof(values[1]);
        }
        if(line.find(name3) != std::string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            buffers = stof(values[1]);
        }
    }
    return (100*(1-(freeMem/(totalMem-buffers))));
}

string ProcessParser::getSysKernelVersion(){
    string line = "";
    string name = "Linux";
    stream = Util::getStream(Path::basePath() + Path::versionPath());
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string>values(beg, end);
            return values[2];
        }
    }
    return name;
}

string ProcessParser::getOSName(){
    stream = Util::getStream("/etc/os-release");
    string OSName = "";
    string name = "PRETTY_NAME=";
    string line = "";
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            OSName = line.substr(name.size());
            return OSName.substr(0, OSName.size()-1);
        }
    }
    return OSName;
}
int ProcessParser::getTotalThreads(){
    string name = "Threads";
    string line= "";
    int totalThreads = 0;
    for(auto pid : ProcessParser::getPidList()){
        stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
        while(getline(stream, line)){
            if(line.find(name) != string::npos){
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                totalThreads += stoi(values[1]);
            }
        }
    }
    return totalThreads;
}

int ProcessParser::getTotalNumberOfProcesses(){
    string name = "processes";
    string line= "";
    int totalProcesses = 0;
    stream = Util::getStream(Path::basePath() + Path::statPath());
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            totalProcesses = stoi(values[1]);
        }
    }
    return totalProcesses;
}

int ProcessParser::getNumberOfRunningProcesses(){
    string name = "procs running";
    string line= "";
    int totalProcesses = 0;
    stream = Util::getStream(Path::basePath() + Path::statPath());
    while(getline(stream, line)){
        if(line.find(name) != string::npos){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            totalProcesses = stoi(values[2]);
        }
    }
    return totalProcesses;
}