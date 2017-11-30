#ifndef SOCKETLITE_SEDA_STAGE_HANDLER_H
#define SOCKETLITE_SEDA_STAGE_HANDLER_H
#include "SL_Seda_Interface.h"

class SL_Seda_StageHandler : public SL_Seda_IStageHandler
{
public:
    SL_Seda_StageHandler()
    {
    }

    virtual int handle_open()
    {
        return 0;
    }

    virtual int handle_close()
    {
        return 0;
    }

    virtual int handle_event(const SL_Seda_Event *event)
    {
        return 0;
    }

    void set_stage_thread(SL_Seda_IStageThread *stage_thread)
    {
        stage_thread_ = stage_thread;
    }

public:
    SL_Seda_IStageThread *stage_thread_;
};

#endif

