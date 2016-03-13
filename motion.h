// generated

#include "dds.h"

#ifndef _DDSL_MOTION_H_
#define _DDSL_MOTION_H_


typedef struct Motion_coordinate
{
  float x;
  float y;
} Motion_coordinate;

extern const dds_topic_descriptor_t Motion_coordinate_desc;

#define Motion_coordinate__alloc() \
((Motion_coordinate*) dds_alloc (sizeof (Motion_coordinate)));

#define Motion_coordinate_free(d,o) \
dds_sample_free ((d), &Motion_coordinate_desc, (o))

#endif /* _DDSL_MOTION_H_ */