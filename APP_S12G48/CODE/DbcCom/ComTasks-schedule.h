/**
  ******************************************************************************
  /*License         : MIT License. Copyright (c) 2022 IsaacIbm.
  * @author         : IsaacIbm
  * @Email/Hangouts : Isk.ibm@gmail.com
  * @role           : Senior Automotive software engineer
  *                 : For Custom Embedded software please dont hesitate to
  *                 : contact me isk.ibm@gmail.com
  *                 : at https://www.freelancer.com/u/IsaacIbm
  ******************************************************************************
**/



#ifndef __COMTASKS_SCHEDULE_H
#define __COMTASKS_SCHEDULE_H

#include "Com_Tasks.h"
#include "PE_Types.h"
#include "stddef.h"

/* Prototypes for tasks
 *
 * There may be at most one function called for each time slot. The function you
 * define can call as many other functions as you like, but be cognizant of the
 * amount of time consumed by each.  For example: calling a functions that takes
 * 32ms to execute every 32ms will not leave any time for the 2ms, 4ms, 8ms, 16ms
 * tasks to execute without causing time to "slip" and tasks *skipped* the next time
 * the scheduler runs to catch up with the time pattern.
 *
 * On a given slot, will call the maximum time, then each match in descending order,
 * For example, at 16ms: the 16ms slot, the 8ms slot, the 4ms slot, and 2ms slot will
 * be called in that order.
 *
 *
*/

#define TASK_2MS               (ComTask_2ms        )
#define TASK_4MS               (ComTask_4ms        )
#define TASK_8MS               (ComTask_8ms        )
#define TASK_16MS              (ComTask_16ms       )
#define TASK_32MS              (ComTask_32ms       )
#define TASK_64MS              (ComTask_64ms       )
#define TASK_128MS             (ComTask_128ms      )
#define TASK_256MS             (ComTask_256ms      )
#define TASK_512MS             (ComTask_512ms      )
#define TASK_1024MS            (ComTask_1024ms     )
#define TASK_2048MS            (ComTask_2048ms     )
#define TASK_4096MS            (ComTask_4096ms     )
#define TASK_8192MS            (ComTask_8192ms     )
#define TASK_16384MS           (ComTask_16384ms    )
#define TASK_32768MS           (ComTask_32768ms    )
#define TASK_65536MS           (ComTask_65536ms    )
#define TASK_131072MS          (ComTask_131072ms   )
#define TASK_262144MS          (ComTask_262144ms   )
#define TASK_528288MS          (ComTask_528288ms   )
#define TASK_1056576MS         (ComTask_1056576ms  )
#define TASK_2113152MS         (ComTask_2113152ms  )
#define TASK_4226304MS         (ComTask_4226304ms  )
#define TASK_8452608MS         (ComTask_8452608ms  )
#define TASK_16905216MS        (ComTask_16905216ms )
#define TASK_33810432MS        (ComTask_33810432ms )
#define TASK_67620864MS        (ComTask_67620864ms )
#define TASK_135241728MS       (ComTask_135241728ms)


// Given time in ms, call tasks on powers of 2 ms (2,4,8,16,32,64,128,256,512,1024,2048,4096...)
// On a given slot, will call the maximum time, then in descending order,
// For example, at 16ms, the 16ms slot, the 8ms slot, the 4ms slot, and 2ms slot will be called in
// that order.
void ComTasksOnPowerOfTwo(uint32_t TimeMs);

// From the LSB, count the number of zero's using black magic.
uint16_t CountZerosFromLSB(uint32_t v);



#endif // __COMTASKS_SCHEDULE_H
