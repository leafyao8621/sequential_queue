#ifndef _ENGINE_EVENTS_H_
#define _ENGINE_EVENTS_H_

int Event_arrive(struct Event *e, unsigned id);
int Event_start_working(struct Event *e, unsigned station_idx);
int Event_transfer(struct Event *e, unsigned from, unsigned idx);
int Event_depart(struct Event *e, unsigned idx);

#endif
