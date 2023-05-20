#include "TaskScheduler.h"

void TaskScheduler::Check()
{
    uint32_t now = HAL_GetTick();
    for (uint8_t i = 0; i < TASKER_MAX_TASKS; i++)
    {

        Task *t = &tasks[i];

        if (t->cb == NULL)
            continue;

        uint32_t lastRunDiff = now - t->lastRun;
        if (t->interval != 0)
        {
            if (lastRunDiff >= t->delay && lastRunDiff >= t->interval)
            {
                t->lastRun += t->interval;

                uint8_t returnCode = (*(TaskCallback)(t->cb))();
                if (TaskScheduler::observer != NULL)
                    TaskScheduler::observer(t->taskId, returnCode);

                if (t->interval == 0) // One Shot Task
                    t->cb = NULL;
            }
        }
        else
        {
            if (lastRunDiff >= t->delay)
            {
                uint8_t returnCode = (*(TaskCallback)(t->cb))();
                if (TaskScheduler::observer != NULL)
                    TaskScheduler::observer(t->taskId, returnCode);

                t->cb = NULL;
            }
        }

        now = HAL_GetTick();
    }
}

void TaskScheduler::RemoveAll()
{
    for (uint8_t i = 0; i < TASKER_MAX_TASKS; i++)
    {
        Task *ptr = &tasks[i];
        ptr->cb = NULL;
    }
}

Task *TaskScheduler::findSlot()
{
    for (uint8_t i = 0; i < TASKER_MAX_TASKS; i++)
    {
        Task *ptr = &tasks[i];
        if (ptr->cb == NULL)
            return ptr;
    }

    return NULL;
}

void TaskScheduler::schedule(TaskCallback cb, uint32_t interval, uint32_t delay, uint8_t taskId)
{
    Task *t = findSlot();

    if (t == NULL)
        return;

    t->cb = cb;
    t->interval = interval;
    t->delay = delay;
    t->taskId = taskId;
    t->lastRun = HAL_GetTick();
}

void TaskScheduler::ScheduleTask(TaskCallback cb, uint32_t delay)
{
    schedule(cb, 0, delay, 0);
}

void TaskScheduler::ScheduleTask(TaskCallback cb, uint32_t delay, uint8_t taskId)
{
    schedule(cb, 0, delay, taskId);
}

void TaskScheduler::SchedulePeriodicTask(TaskCallback cb, uint32_t interval, uint32_t delay)
{
    schedule(cb, interval, delay, 0);
}

void TaskScheduler::SchedulePeriodicTask(TaskCallback cb, uint32_t interval, uint32_t delay, uint8_t taskId)
{
    schedule(cb, interval, delay, taskId);
}

void TaskScheduler::RemoveTask(TaskCallback cb)
{
    for (uint8_t i = 0; i < TASKER_MAX_TASKS; i++)
    {
        Task *ptr = &tasks[i];
        if (ptr->cb == cb)
        {
            ptr->cb = NULL;
            return;
        }
    }
}

void TaskScheduler::RegisterObserver(SignalCallback cb)
{
    TaskScheduler::observer = cb;
}