#ifndef SOCKETLITE_SINGLETON_H
#define SOCKETLITE_SINGLETON_H

template <typename T>
class SL_Singleton
{
public:
    static T* instance()
    {
        static T object_type;
        return &object_type;
    }

private:
    inline SL_Singleton() 
    {
    }

    inline ~SL_Singleton() 
    {
    }

};

#endif

