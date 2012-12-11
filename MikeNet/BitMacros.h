#pragma once

#define BIT_ON(Variable,BitNumber) ((Variable) |= (1<<(BitNumber)))
#define BIT_OFF(Variable,BitNumber) ((Variable) &= ~(1<<(BitNumber)))
#define BIT_TOGGLE(Variable,BitNumber) ((Variable) ^= (1<<(BitNumber)))
#define BIT_GET(Variable,BitNumber)  (((Variable) & (1<<(BitNumber))) > 0)

#define BITMASK_ON(x,y) ((x) |= (y))
#define BITMASK_OFF(x,y) ((x) &= (~(y)))
#define BITMASK_TOGGLE(x,y) ((x) ^= (y))
#define BITMASK_GET(x,y) (((x) & (y)) > 0)