#include "L_file.h"

Lfile::Lfile()
{
    this->filename.reserve(32);
    this->file = std::fstream();
}

void Lfile::open_file()
{
    /*
    //the below code will always pass, but it is not needed,
    //since it is easy to ensure a nofile never reaches this function
    if(this->type != Lfile_file_category::NOFILE)
    {
        this->close_file();
    }
    */

    //REALLY dislike using if/else here!
    if(this->type == Lfile_file_category::BINARY)
    {
        this->file.open(this->filename,std::ios::binary | std::fstream::out | std::fstream::in);
        if(!file)
        {
            this->file.open(
                this->filename,std::ios::binary | std::fstream::out | std::fstream::in | std::fstream::trunc
                );
            if(!file)
            {
                this->close_file();
            }
        }
    }
    else if(this->type == Lfile_file_category::REGULAR)
    {
        this->file.open(this->filename,std::fstream::out | std::fstream::in);
        if(!file)
        {
            this->file.open(
                this->filename,std::fstream::out | std::fstream::in | std::fstream::trunc
                );
            if(!file)
            {
                this->close_file();
            }
        }
    }
    else
    {
        //todo error code here
    }
}

void Lfile::open_file_app()
{
    //could have been through if else in open_file() func, but this will be a fairly common case and I find that to be too many if/else
        if(this->type == Lfile_file_category::BINARY)
    {
        this->file.open(this->filename,std::ios::binary | std::fstream::out | std::fstream::in | std::ios::app);
        if(!file)
        {
            this->file.open(
                this->filename,std::ios::binary | std::fstream::out | std::fstream::in | std::fstream::trunc
                );
            if(!file)
            {
                this->close_file();
            }
        }
    }
    else if(this->type == Lfile_file_category::REGULAR)
    {
        this->file.open(this->filename,std::fstream::out | std::fstream::in | std::ios::app);
        if(!file)
        {
            this->file.open(
                this->filename,std::fstream::out | std::fstream::in | std::fstream::trunc
                );
            if(!file)
            {
                this->close_file();
            }
        }
    }
    else
    {
        //todo error code here
    }
}

void Lfile::create(std::string fileDir, Lfile_file_category fileType)
{
    this->filename = fileDir;
    if(this->type == Lfile_file_category::BINARY)
    {
        this->file.open(this->filename,std::ios::binary | std::fstream::out | std::fstream::in | std::ios::trunc);
    }
    else if(this->type == Lfile_file_category::REGULAR)
    {
        this->file.open(this->filename,std::fstream::out | std::fstream::in | std::ios::trunc);
    }
    else
    {
        //todo error code
    }
}

void Lfile::clear_file()
{
    this->nofile_errorcheck();
    this->file.close();
    this->create(this->filename,this->type);
}

void Lfile::close_file()
{
    if(this->type != Lfile_file_category::NOFILE)
    {
        this->file.close();
        this->filename.clear();
        //not shrinking filename to fit
        this->type = Lfile_file_category::NOFILE;
    }
}

void Lfile::nofile_errorcheck()
{
    if(this->type == Lfile_file_category::NOFILE)
    {
        //todo error code here
    }
}

void Lfile::open(std::string fileDir,Lfile_file_category fileType, bool append)
{
    this->filename = fileDir;
    this->filename.shrink_to_fit();
    this->type = fileType;
    if(append == true)
        open_file_app();
    else
        open_file();
}

void Lfile::openBinary(std::string fileDir, bool append)
{
    this->filename = fileDir;
    this->filename.shrink_to_fit();
    this->type = Lfile_file_category::BINARY;
    if(append == true)
        open_file_app();
    else
        open_file();
}

void Lfile::openRegular(std::string fileDir, bool append)
{
    this->filename = fileDir;
    this->filename.shrink_to_fit();
    this->type = Lfile_file_category::REGULAR;
    if(append == true)
        open_file_app();
    else
        open_file();
}

unsigned long long Lfile::getPos()
{
    this->nofile_errorcheck();
    return this->file.tellg();
}

void Lfile::setPos(unsigned long long offset)
{
    this->nofile_errorcheck();
    this->file.seekg(std::ios::cur + offset);
}

void Lfile::setPosAbs(unsigned long long pos)
{
    this->nofile_errorcheck();
    this->file.seekg(std::ios::beg + pos);
}


void Lfile::write(char* data,unsigned long long offsetFromCurr)
{
    this->nofile_errorcheck();
    this->file.seekp(std::ios::cur -1 + offsetFromCurr);
    this->file << data;
}

void Lfile::write(std::string data, unsigned long long offsetFromCurr)
{
    this->nofile_errorcheck();
    this->file.seekp(std::ios::cur -1 + offsetFromCurr);
    this->file.write(&data[0],data.size());    
}

void Lfile::write(std::string *data, unsigned long long offsetFromCurr)
{
    this->nofile_errorcheck();
    this->file.seekp(std::ios::cur -1 + offsetFromCurr);
    this->file.write(&((*data)[0]),data->size());
}


template<typename T>
void Lfile::write(T* data,unsigned long long offsetFromCurr)
{
    this->nofile_errorcheck();
    this->file.seekp(std::ios::cur + -1 + offsetFromCurr);
    this->file.write(static_cast<char*>(static_cast<void*>(data)), sizeof(T));
}

template<typename T>
void Lfile::write(std::vector<T>& data,unsigned long long offsetFromCurr)
{
    this->nofile_errorcheck();
    this->file.seekp(std::ios::cur + -1 + offsetFromCurr);
    this->file.write(static_cast<char*>(static_cast<void*>(&data[0])), sizeof(T)*data.size());
}

template<typename T>
void Lfile::write(T* data, unsigned long long length,unsigned long long offsetFromCurr)
{
    this->nofile_errorcheck();
    this->file.seekp(std::ios::cur + -1 + offsetFromCurr);
    this->file.write(static_cast<char*>(static_cast<void*>(data)), sizeof(T)*length);
}


template<typename T>
void Lfile::writeat(T* data,unsigned long long pos)
{
    this->nofile_errorcheck();
    this->file.seekp(std::ios::beg + pos);
    this->file.write(static_cast<char*>(static_cast<void*>(data)), sizeof(T) );
}

template<typename T>
void Lfile::writeat(std::vector<T>& data,unsigned long long pos)
{
    this->nofile_errorcheck();
    this->file.seekp(std::ios::beg + pos);
    this->file.write(static_cast<char*>(static_cast<void*>(&data[0])), sizeof(T)*data.size());
}

template<typename T>
void Lfile::writeat(T* data, unsigned long long length,unsigned long long pos)
{
    this->nofile_errorcheck();
    this->file.seekp(std::ios::beg + pos);
    this->file.write(static_cast<char*>(static_cast<void*>(data)), sizeof(T)*length);
}



int main()
{
    Lfile myfile;
    myfile.openBinary(R"(firstLfile.txt)",true);
    std::vector<char> a = {'a','l','l','e','q'};
    char* b = "hellos";
    //todo: make it possible to write at offsets from begining, from end, from current pos
    //      as well as make it possible to get and change value of file pointer
    myfile.write(b);
    return 1;
}