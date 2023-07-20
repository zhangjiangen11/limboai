/* bb_vector2.h */

#ifndef BB_VECTOR2_H
#define BB_VECTOR2_H

#include "bb_param.h"
#include "core/object/object.h"

class BBVector2 : public BBParam {
	GDCLASS(BBVector2, BBParam);

protected:
	virtual Variant::Type get_type() const override { return Variant::VECTOR2; }
};

#endif // BB_VECTOR2_H