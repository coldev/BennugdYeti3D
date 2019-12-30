
/*
** Name: pickup_tick
** Desc: A standard pickup behaviour. Entity is placed at a choosen position
**       from the ground and bobs up & down.
*/
void pickup_tick(yeti_t* yeti, entity_t* e, int z)
{
  cell_t* cell = ENTITY_CELL(yeti, e);
  e->z = cell_bot(cell, e) + (fixsin(yeti->keyboard.tick << 12) >> 3) + z;
}
