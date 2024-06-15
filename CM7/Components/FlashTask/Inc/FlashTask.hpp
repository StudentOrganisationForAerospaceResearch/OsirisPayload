/**
 ******************************************************************************
 * File Name          : FlashTask.hpp
 * Description        : Flash interface task. Used for data logging and state recovery
 ******************************************************************************
*/
#ifndef FLASHTASK_HPP_
#define FLASHTASK_HPP_

#include "Task.hpp"
#include "SystemDefines.hpp"
#include "W25N01GVSFIG.hpp"

/* Macros/Enums ------------------------------------------------------------*/
enum FLASH_COMMANDS {
  WRITE_STATE_TO_FLASH = 0,
  WRITE_DATA_TO_FLASH = 0x31,
  DUMP_FLASH_DATA = 0x50,
  ERASE_ALL_FLASH = 0x60,
};

class FlashTask : public Task
{
public:
  static FlashTask& Inst() {
    static FlashTask inst;
    return inst;
  }

  void InitTask();

protected:
  static void RunTask(void* pvParams) { FlashTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

  void Run(void * pvParams); // Main run code

  void HandleCommand(Command& cm);

  // Log Data Functions
  void WriteLogDataToFlash(uint8_t* data, uint16_t size);
  bool ReadLogDataFromFlash();

private:
  // Private Functions
  FlashTask();        // Private constructor
  FlashTask(const FlashTask&);                        // Prevent copy-construction
  FlashTask& operator=(const FlashTask&);            // Prevent assignment

  // Offsets
  struct Offsets
  {
    uint32_t writeDataOffset;
  };

  Offsets currentOffsets_;

  W25N01GVSFIG flashChipHandle = {GPIOB, GPIO_PIN_10, nullptr, 0, 0};
};

#endif    // SOAR_FLASHTASK_HPP_
