
#include "snet/file.h"
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc,char** argv)
{
    snet::File f("test.txt");
    assert(f.open(0) < 0);
    assert(f.open(O_RDWR|O_CREAT) > 0);
    std::string test = "hello world!";
    assert(f.write(test.c_str(),test.size()) == test.size());
    assert(f.seek(0) == 0);
    assert(f.len() == test.size());
    char buf[1024];
    int len = f.read(buf,1024);
    assert(len == test.size());
    assert(f.remove() == 0);
    assert(!f.exist());
}
