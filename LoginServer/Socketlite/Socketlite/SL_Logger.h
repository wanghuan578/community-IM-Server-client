/////////////////////////////////////////////////////////////////////////
//
//Logger switch for use ..
//

#ifndef SOCKETLITE_LOGGER_H
#define SOCKETLITE_LOGGER_H

#include "SL_Config.h"
#ifdef SOCKETLITE_LOG_USE_LOG4CPLUS   //use log4cpluse..
	#include <log4cplus/loggingmacros.h>
    #include <log4cplus/logger.h>
    #include <log4cplus/configurator.h>

    #define SL_LOGGER_TEXT LOG4CPLUS_TEXT
    #define SL_LOGGER_SYS_INIT(property_file) log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(property_file))
    #define SL_LOGGER_SYS_INIT_DYNAMIC(property_file, check_interval_ms) log4cplus::ConfigureAndWatchThread log4cplus_configThread(LOG4CPLUS_TEXT(property_file), check_interval_ms)
    
    #define SL_LOGGER_CLASS_DECL(logger) static log4cplus::Logger logger
    #define SL_LOGGER_CLASS_IMPL(logger, classname) log4cplus::Logger classname::logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(#classname))

    #define SL_LOGGER_EXTERN_DECL(logger)  extern log4cplus::Logger logger
    #define SL_LOGGER_IMPL(logger, name)   log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(name))
    #define SL_LOGGER_STATIC_DECL_IMPL(logger, name) static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(name))
    #define SL_LOGGER_STATIC_DECL_IMPL_ROOT(logger)  static log4cplus::Logger logger = log4cplus::Logger::getRoot()

    #define SL_LOG_TRACE(logger,e)  LOG4CPLUS_TRACE(logger,e)
    #define SL_LOG_DEBUG(logger,e)  LOG4CPLUS_DEBUG(logger,e)
    #define SL_LOG_INFO(logger,e)   LOG4CPLUS_INFO(logger,e)
    #define SL_LOG_WARN(logger,e)   LOG4CPLUS_WARN(logger,e)
    #define SL_LOG_ERROR(logger,e)  LOG4CPLUS_ERROR(logger,e)
    #define SL_LOG_FATAL(logger,e)  LOG4CPLUS_FATAL(logger,e)

#elif defined(SOCKETLITE_LOG_USE_STDOUT) //use simple stdout..
    //#include <iostream>
    #include <sstream>
    #include <time.h>
    #include <sys/time.h>
    //don't use this...
    //#define SOCKETLITE_LOGGER_SYS_OUT(logger,e,tag)  logger <<<<"["<<__FILE__<<":"<<__LINE__<<"] ["<<tag<<"] "<<e<<std::endl;
    #define SOCKETLITE_LOGGER_SYS_OUT(logger,e,tag)  \
                    do{\
                        std::ostringstream oss; \
                        oss<<e;\
                        char buff[64]; struct tm stm; struct timeval tv; struct timezone tz; \
                        gettimeofday(&tv, &tz); \
                        strftime(buff,sizeof(buff),"%y%m%d %T",localtime_r(&tv.tv_sec,&stm));\
                        fprintf(stdout,"[%s.%06ld][%s:%d] [%s] %s\n",buff,tv.tv_usec,__FILE__,__LINE__,tag,oss.str().c_str());\
                        fflush(stdout);\
                    }while(0);

    #define SL_LOGGER_SYS_INIT(property_file)
    #define SL_LOGGER_SYS_INIT_DYNAMIC(property_file,check_interval_ms) 
    
    #define SL_LOGGER_CLASS_DECL(logger) 
    #define SL_LOGGER_CLASS_IMPL(logger,classname)

    #define SL_LOGGER_EXTERN_DECL(logger)  
    #define SL_LOGGER_IMPL(logger, name) 
    #define SL_LOGGER_STATIC_DECL_IMPL(logger,name)

    #define SL_LOG_TRACE(logger,e)  SOCKETLITE_LOGGER_SYS_OUT(std::cout,e,"TRACE");
    #define SL_LOG_DEBUG(logger,e)  SOCKETLITE_LOGGER_SYS_OUT(std::cout,e,"DEBUG");
    #define SL_LOG_INFO(logger,e)   SOCKETLITE_LOGGER_SYS_OUT(std::cout,e,"INFO");
    #define SL_LOG_WARN(logger,e)   SOCKETLITE_LOGGER_SYS_OUT(std::cerr,e,"WARN");
    #define SL_LOG_ERROR(logger,e)  SOCKETLITE_LOGGER_SYS_OUT(std::cerr,e,"ERROR");
    #define SL_LOG_FATAL(logger,e)  SOCKETLITE_LOGGER_SYS_OUT(std::cerr,e,"FATAL");
            
#else //no use logger..
    #define SL_LOGGER_SYS_INIT(property_file)
    #define SL_LOGGER_SYS_INIT_DYNAMIC(property_file,check_interval_ms) 
    
    #define SL_LOGGER_CLASS_DECL(logger) 
    #define SL_LOGGER_CLASS_IMPL(logger,classname)

    #define SL_LOGGER_EXTERN_DECL(logger)  
    #define SL_LOGGER_IMPL(logger, name) 
    #define SL_LOGGER_STATIC_DECL_IMPL(logger,name)

    #define SL_LOG_TRACE(logger,e) 
    #define SL_LOG_DEBUG(logger,e)  
    #define SL_LOG_INFO(logger,e)   
    #define SL_LOG_WARN(logger,e) 
    #define SL_LOG_ERROR(logger,e)  
    #define SL_LOG_FATAL(logger,e)
#endif

//--------------------------------------------log level complie control..
#ifdef SOCKETLITE_LOG_LEVEL_OFF
    #define SOCKETLITE_LOG_LEVEL_FATAL
    
    #undef SL_LOG_FATAL
    #define SL_LOG_FATAL(a,b)
#endif 

#ifdef SOCKETLITE_LOG_LEVEL_FATAL
    #define SOCKETLITE_LOG_LEVEL_ERROR
    
    #undef SL_LOG_ERROR
    #define SL_LOG_ERROR(a,b)
#endif 

#ifdef SOCKETLITE_LOG_LEVEL_ERROR
    #define SOCKETLITE_LOG_LEVEL_WARN
    
    #undef SL_LOG_WARN
    #define SL_LOG_WARN(a,b)
#endif 

#ifdef SOCKETLITE_LOG_LEVEL_WARN
    #define SOCKETLITE_LOG_LEVEL_INFO
    
    #undef SL_LOG_INFO
    #define SL_LOG_INFO(a,b)
#endif 

#ifdef SOCKETLITE_LOG_LEVEL_INFO
    #define SOCKETLITE_LOG_LEVEL_DEBUG
    
    #undef SL_LOG_DEBUG
    #define SL_LOG_DEBUG(a,b)
#endif 

#ifdef SOCKETLITE_LOG_LEVEL_DEBUG
    #define SOCKETLITE_LOG_LEVEL_TRACE
    
    #undef SL_LOG_TRACE
    #define SL_LOG_TRACE(a,b)
#endif 

#endif

