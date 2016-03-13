// generated

#include "motion.h"


static const uint32_t Motion_coordinate_ops [] =
{
  DDS_OP_ADR | DDS_OP_TYPE_4BY, offsetof (Motion_coordinate, x),
  DDS_OP_ADR | DDS_OP_TYPE_4BY, offsetof (Motion_coordinate, y),
  DDS_OP_RTS
};

const dds_topic_descriptor_t Motion_coordinate_desc =
{
  sizeof (Motion_coordinate),
  4u,
  0u,
  0u,
  "Motion::coordinate",
  NULL,
  Motion_coordinate_ops,
  "<MetaData version=\"1.0.0\"><Module name=\"Motion\"><Struct name=\"coordinate\"><Member name=\"x\"><Float/></Member><Member name=\"y\"><Float/></Member></Struct></Module></MetaData>"
};