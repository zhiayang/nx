// timer.s
// Copyright (c) 2019, zhiayang
// Licensed under the Apache License Version 2.0.

.global nx_x64_timer_tick
.type nx_x64_timer_tick, @function

// this is called every time the timer ticks.
// we want it to call some scheduler function that increases
// some tick counter or whatever -- then the scheduler is the one that actually calls the
// thread switcher (switch.s). we don't want to wire the timer directly to the thread switcher
// for portability and/or extensibility reasons. we might end up using the HPET in the future, which
// can get high frequencies -- good for keeping track of time slices accurately, but probably not necessary
// (or even detrimental) to switch contexts so often.

nx_x64_timer_irq:
