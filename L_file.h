//Supposed to be a very abstract and easy way to manage files

#include <string>
#include <fstream>
#include <vector>

enum class Lfile_file_category
{
    NOFILE,
    REGULAR,
    BINARY
};


//template <class T>    //not a good solution at all.
class Lfile{
private:
    std::string filename;
    std::fstream file;
    Lfile_file_category type = Lfile_file_category::NOFILE;

    void open_file();
    void open_file_app();
    void nofile_errorcheck();
public:
    Lfile();
    //~Lfile();     //not needed as of now

    void create(std::string fileDir,Lfile_file_category fileType);
    void open(std::string fileDir,Lfile_file_category fileType,bool append = false);
    void openRegular(std::string fileDir,bool append = false);
    void openBinary(std::string fileDir, bool append = false);

    unsigned long long getPos();
    void setPos(unsigned long long offset);
    void setPosAbs(unsigned long long pos);

    void clear_file();
    void close_file();


    //do not like how the user has to deal with complex classes themselves
    //but taking care of that is not possible since diff classes require specific solutions
    //another thing I do not like is that the  write(T* data) function 
    //will assume that only one element is being written, So I am considering to remove it.

    //TODO meke the offsets and pos handle problems in a good way
    void write(char* data,unsigned long long offsetFromCurr = 0);
    void write(std::string data,unsigned long long offsetFromCurr = 0);
    void write(std::string* data, unsigned long long offsetFromCurr = 0);
    template<typename T>
    void write(T* data,unsigned long long offsetFromCurr = 0);
    template<typename T>
    void write(std::vector<T>& data,unsigned long long offsetFromCurr = 0);
    template<typename T>
    void write(T* data, unsigned long long length,unsigned long long offsetFromCurr = 0);

    template<typename T>
    void writeat(T* data,unsigned long long pos);
    template<typename T>
    void writeat(std::vector<T>& data,unsigned long long pos);
    template<typename T>
    void writeat(T* data, unsigned long long length,unsigned long long pos);
};