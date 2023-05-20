#ifndef TaskScheduler_h
#define TaskScheduler_h

#include <math.h>

#include "hal_driver.h"
#include "Config.h"

typedef uint8_t (*TaskCallback)(void);
typedef void (*SignalCallback)(uint8_t id, uint8_t returnCode);
typedef struct Task
{
    uint32_t interval;
    uint32_t lastRun;
    uint32_t delay;
    TaskCallback cb;
    uint8_t taskId;
} task;

class TaskScheduler
{
public:
    static void Check();
    static void ScheduleTask(TaskCallback cb, uint32_t delay);
    static void ScheduleTask(TaskCallback cb, uint32_t delay, uint8_t taskId);
    static void SchedulePeriodicTask(TaskCallback cb, uint32_t interval, uint32_t delay);
    static void SchedulePeriodicTask(TaskCallback cb, uint32_t interval, uint32_t delay, uint8_t taskId);
    static void RemoveTask(TaskCallback cb);
    static void RemoveAll();
    static void RegisterObserver(SignalCallback cb);

private:
    TaskScheduler(){};
    inline static Task tasks[TASKER_MAX_TASKS];
    inline static SignalCallback observer = NULL;

    static void schedule(TaskCallback cb, uint32_t interval, uint32_t delay, uint8_t taskId);
    static Task *findSlot();
};

#endif