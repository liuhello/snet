
#ifndef __MISC_H__
#define __MISC_H__

namespace snet
{
    /*
    *   set copy method private and undefine to disable copy.
    **/
    class NonCopyable
    {
    public:
        NonCopyable(){}
        ~NonCopyable(){}
    private:
        NonCopyable(const NonCopyable&n);
        const NonCopyable& operator = ( const NonCopyable& );
    };
}

#endif

