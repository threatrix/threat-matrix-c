/*
 * Copyright (C) 2002 ETC s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by Marcel Telka <marcel@telka.sk>, 2002.
 * Rewritten in c++ by Rus V. Brushkoff <rus@soyuzkt.od.ua>, 2004.
 *
 */

#ifndef	_STATE_H_
#define	_STATE_H_

//#include <common.h>

#define	TAPSTAT_DR	(1 << 0)
#define	TAPSTAT_IR	(1 << 1)
#define	TAPSTAT_SHIFT	(1 << 2)		/* register shift with TMS = 0 */
#define	TAPSTAT_IDLE	(1 << 3)		/* to Run-Test/Idle with TMS = 0 */
#define	TAPSTAT_CAPTURE	(1 << 4)		/* Capture state */
#define	TAPSTAT_UPDATE	(1 << 5)		/* to Update with TMS = 1 */
#define	TAPSTAT_PAUSE	(1 << 6)		/* to Pause with TMS = 0 */
#define	TAPSTAT_RESET	(1 << 7)		/* Test-Logic-Reset or unknown state */

#define	Unknown_State		TAPSTAT_RESET
#define	Test_Logic_Reset	(TAPSTAT_RESET | TAPSTAT_IDLE)
#define	Run_Test_Idle		TAPSTAT_IDLE
#define	Select_DR_Scan		TAPSTAT_DR
#define	Capture_DR		(TAPSTAT_DR | TAPSTAT_SHIFT | TAPSTAT_CAPTURE)
#define	Shift_DR		(TAPSTAT_DR | TAPSTAT_SHIFT)
#define	Exit1_DR		(TAPSTAT_DR | TAPSTAT_UPDATE | TAPSTAT_PAUSE)
#define	Pause_DR		(TAPSTAT_DR | TAPSTAT_PAUSE)
#define	Exit2_DR		(TAPSTAT_DR | TAPSTAT_SHIFT | TAPSTAT_UPDATE)
#define	Update_DR		(TAPSTAT_DR | TAPSTAT_IDLE)
#define	Select_IR_Scan		TAPSTAT_IR
#define	Capture_IR		(TAPSTAT_IR | TAPSTAT_SHIFT | TAPSTAT_CAPTURE)
#define	Shift_IR		(TAPSTAT_IR | TAPSTAT_SHIFT)
#define	Exit1_IR		(TAPSTAT_IR | TAPSTAT_UPDATE | TAPSTAT_PAUSE)
#define	Pause_IR		(TAPSTAT_IR | TAPSTAT_PAUSE)
#define	Exit2_IR		(TAPSTAT_IR | TAPSTAT_SHIFT | TAPSTAT_UPDATE)
#define	Update_IR		(TAPSTAT_IR | TAPSTAT_IDLE)

class jtag_state {
  int state_;
 public:
  jtag_state() { state_ = Unknown_State; }
  void state(int s) { state_ = s; }
  int state() { return state_; }
  int clock(int tms) {
    if (tms) {
      switch (state_) {
        case Test_Logic_Reset:
          break;
        case Run_Test_Idle:
        case Update_DR:
        case Update_IR:
          state_ = Select_DR_Scan;
          break;
        case Select_DR_Scan:
          state_ = Select_IR_Scan;
          break;
        case Capture_DR:
        case Shift_DR:
          state_ = Exit1_DR;
          break;
        case Exit1_DR:
        case Exit2_DR:
           state_ = Update_DR;
           break;
        case Pause_DR:
           state_ = Exit2_DR;
           break;
        case Select_IR_Scan:
           state_ = Test_Logic_Reset;
           break;
        case Capture_IR:
        case Shift_IR:
           state_ = Exit1_IR;
           break;
        case Exit1_IR:
        case Exit2_IR:
           state_ = Update_IR;
           break;
        case Pause_IR:
           state_ = Exit2_IR;
           break;
        default:
           state_ = Unknown_State;
           break;
      } // switch(state_)
     } else { // if (tms)
      switch (state_) {
        case Test_Logic_Reset:
        case Run_Test_Idle:
        case Update_DR:
        case Update_IR:
          state_ = Run_Test_Idle;
          break;
        case Select_DR_Scan:
          state_ = Capture_DR;
          break;
        case Capture_DR:
        case Shift_DR:
        case Exit2_DR:
          state_ = Shift_DR;
          break;
        case Exit1_DR:
        case Pause_DR:
          state_ = Pause_DR;
          break;
        case Select_IR_Scan:
          state_ = Capture_IR;
          break;
        case Capture_IR:
        case Shift_IR:
        case Exit2_IR:
          state_ = Shift_IR;
          break;
        case Exit1_IR:
        case Pause_IR:
          state_ = Pause_IR;
          break;
        default:
          state_ = Unknown_State;
          break;
    } // switch(state_)
  } // if (!tms)
   return state_;
 }
  int trst(int old_trst, int new_trst) {
    old_trst = old_trst ? 1 : 0;
    new_trst = new_trst ? 1 : 0;
    if (old_trst != new_trst) {
      if (new_trst)
        state_ = Test_Logic_Reset;
      else
        state_ = Unknown_State;
   }
    return state_;
 }
};

#endif
