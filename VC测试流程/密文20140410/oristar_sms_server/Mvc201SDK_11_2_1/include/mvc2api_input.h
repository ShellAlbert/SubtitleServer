#ifndef _MVC2API_INPUT_H_
#define _MVC2API_INPUT_H_

#include "mvc2api_types.h"
#include "mvc2api_device.h"

namespace mvc2
{
	class VideoInputPrivate;

	class MVC2_API VideoInput
	{
	public:
		typedef enum
		{
			INMODE_3G_DUAL				= (1<<0),
			INMODE_2K					= (0<<1),
			INMODE_2K_3D				= (1<<1),
			INMODE_S372_DUAL			= (2<<1),
			INMODE_S372_DUAL_3D		= (3<<1),
			INMODE_4K					= (4<<1),
			INMODE_4K_LEVEL_B			= (5<<1),
			INMODE_4K_QUAD				= (6<<1),
		} INPUT_MODE;

	public:
		VideoInput();
		VideoInput(const VideoInput &other);
		VideoInput(TMmRc * resultPointer, const MvcDevice &mvcDevice);

		virtual ~VideoInput();

		TMmRc setInputMode(uint32_t input_sel, uint32_t input_mode);
		TMmRc getInputMode(uint32_t *input_sel, uint32_t *input_mode);

		TMmRc enableInput(bool enable);

		VideoInput &operator =(const VideoInput &vin);
		operator bool() const;


	protected:
		VideoInput(VideoInputPrivate *dev);
		VideoInputPrivate *m_inputPrivate;
	};
}

#endif /* _MVC2API_INPUT_H_ */
