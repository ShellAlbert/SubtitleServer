#ifndef _MVC2API_OVERLAY_H_
#define _MVC2API_OVERLAY_H_

#include "mvc2api_decoder.h"

namespace mvc2
{
	class OverlayDecoderPrivate;
	class SubtitleDecoderPrivate;
	class OverlayDataBuffer;
	class OverlayDecoder;
	class SubtitleDecoder;

	class MVC2_API OverlayRenderCommand
	{
	public:
		OverlayRenderCommand();
		virtual ~OverlayRenderCommand();

		void clearRenderArea(uint32_t color);

	protected:
		virtual TMmRc renderIntoBuffer(MemoryBuffer *membuf) const = 0;

	protected:
		uint32_t m_flags;
		uint32_t m_clearColor;
		uint32_t m_reserved1;
		uint32_t m_reserved2[4];

		friend class OverlayDataBuffer;
	};

	class MVC2_API RenderPicture : public OverlayRenderCommand
	{
	public:
		RenderPicture(const uint8_t *pictureBuffer, uint32_t size);
		virtual ~RenderPicture();

		void setPosition(const FramePosition &position);

	protected:
		virtual TMmRc renderIntoBuffer(MemoryBuffer *membuf) const;

	private:
		const uint8_t *m_picture_buffer;
		uint32_t m_picture_size;
		FramePosition m_position;
		uint32_t m_reserved2[4];
	};

	class MVC2_API RenderText : public OverlayRenderCommand
	{
	public:
		enum TextFlags
		{
			TextFlag_Italic			= (1<<0),
			TextFlag_Bold				= (1<<1),
			TextFlag_Underline		= (1<<2),
			TextFlag_Strikethrough	= (1<<3),
			TextFlag_Border			= (1<<4),
			TextFlag_Shadow			= (1<<5),
			TextFlag_SoftShadow		= (1<<6),
		};

	public:
		RenderText(const char *textBuffer, uint32_t size = 0);
		virtual ~RenderText();

		void setPosition(const FramePosition &position);

		void setFont(const char *fontName, uint16_t	fontSize, uint16_t textFlags = 0);

		void setColor(uint32_t textColor, uint32_t shadowColor, uint32_t borderColor);

	protected:
		virtual TMmRc renderIntoBuffer(MemoryBuffer *membuf) const;

	private:
		const char *m_text_buffer;
		uint32_t m_text_size;
		FramePosition m_position;
		const char *m_fontName;
		uint16_t m_fontSize;
		uint16_t m_textFlags;
		uint32_t m_textColor;
		uint32_t m_shadowColor;
		uint32_t m_borderColor;
		uint32_t m_reserved2[3];
	};

	class MVC2_API RenderFill : public OverlayRenderCommand
	{
	public:
		RenderFill();
		virtual ~RenderFill();

		void setBox(const FramePosition &startPos, const FramePosition &endPos);

		void setColor(uint32_t fillColor);

	protected:
		virtual TMmRc renderIntoBuffer(MemoryBuffer *membuf) const;

	private:
		FramePosition m_startPos;
		FramePosition m_endPos;
		uint32_t m_fillColor;
		uint32_t m_reserved2[4];
	};


	class MVC2_API OverlayDataBuffer : public DataBuffer
	{
	public:
		enum MOVEEFFECT
		{
			MoveEffect_Linear			=  0,
			MoveEffect_SlowDown		=  1,
			MoveEffect_SpeedUp		=  2,
		};

		enum EFFECTSPEED
		{
			Speed_Immediately			= 0,			/* no transition effect */
			Speed_ExtremelyFast		= 1,			/* approx. 1/16 second */
			Speed_VeryFast				= 4,			/* approx. 1/4 second */
			Speed_Fast					= 8,			/* approx. 1/2 second */
			Speed_Normal				= 16,			/* approx. 1 second */
			Speed_Slow					= 32,			/* approx. 2 seconds */
			Speed_VerySlow				= 48,			/* approx. 3 seconds */
		};

	public:
		OverlayDataBuffer();

		void setDisplayDuration(uint32_t milliseconds);

		void setGlobalAlpha(float alpha, EFFECTSPEED speed = Speed_Immediately, MOVEEFFECT effect = MoveEffect_Linear);

		TMmRc addRenderCommand(const OverlayRenderCommand &cmd);

		void setGlobalPosition(const FramePosition &position, EFFECTSPEED speed = Speed_Immediately, MOVEEFFECT effect = MoveEffect_Linear);

		void setIncompleteRender(bool completion);

		void setSwapBuffer(bool swap);

		void clearScreen();

	protected:
		OverlayDataBuffer(MvcDecoderPrivate *mvcdec, MemoryBuffer *mem);

		friend class OverlayDecoder;
	};


	class MVC2_API OverlayElementDataBuffer : public DataBuffer
	{
	public:
		explicit OverlayElementDataBuffer();

		void setElementName(const char *name, uint32_t resourceId);
		const char *getElementName(uint32_t *resourceId) const;

		void setPartialTransferInfo(uint32_t completeSize, uint32_t offset);

	protected:
		explicit OverlayElementDataBuffer(MvcDecoderPrivate *mvcdec, MemoryBuffer *mem);

		friend class OverlayDecoder;
		friend class SubtitleDecoder;
	};

	class MVC2_API OverlaySubtitleDataBuffer : public DataBuffer
	{
	public:
		explicit OverlaySubtitleDataBuffer();

		void setTimeValues(int32_t timeOffset = 0, uint32_t startTime = 0, uint32_t endTime = UINT32_MAX);

		uint32_t getResourceId() const;

	protected:
		explicit OverlaySubtitleDataBuffer(MvcDecoderPrivate *mvcdec, MemoryBuffer *mem);

		friend class SubtitleDecoder;
	};

	class MVC2_API OverlayDecoder : public MvcDecoder
	{
	public:
		enum RESOLUTION_FLAGS
		{
			Resolution_Absolute			= (0<<0),
			Resolution_Relative			= (1<<0),
			Resolution_Fix_Width			= (0<<1),
			Resolution_Fix_Height		= (1<<1),
			Resolution_Full_Screen		= ((1<<2) | (1<<0))
		};

	public:
		virtual ~OverlayDecoder();

		OverlayDecoder();
		OverlayDecoder(TMmRc * resultPointer, const MvcDevice &mvcdev);
		OverlayDecoder(const OverlayDecoder& other);
		OverlayDecoder& operator= (const OverlayDecoder& other);

		/* @brief Connects an output object to the decoder.
		Description
		This method is used to assign an output to the decoder which
		is used to display the decoder frames.
		@param VideoOut  dedicated VideoOutput object.
		@return MMRC_Ok or an error if the connection fails.
		@retval MMRC_Ok                    \Output successfully
		connected
		@retval MMRC_MVC2_ConnectionError  \Output could not be
		connected, it may not be
		compatible                */
		TMmRc connectOutput(const VideoOutput& VideoOut);

		/* @brief Disconnects the output.
		Description
		Disconnects a previously connected output from the decoder,
		so it can be reused for another decoder.
		@param VideoOut  previously connected output
		@return MMRC_Ok if the disconnection was successful, else an
		error.
		@retval MMRC_Ok                    successful disconnections
		@retval MMRC_MVC2_ConnectionError  object is not connected to
		the decoder                */
		TMmRc disconnectOutput(const VideoOutput& VideoOut);

		virtual TMmRc getDataBuffer(OverlayElementDataBuffer &databuf, uint32_t minsize = 0, uint32_t timeout = WaitInfinite);
		virtual TMmRc getDataBuffer(OverlayDataBuffer &databuf, uint32_t timeout = WaitInfinite);

		TMmRc deleteOverlayElement(const char *elementName);

		TMmRc setOutputResolution(uint32_t width, uint32_t height, uint32_t flags = Resolution_Absolute);

	private:
		explicit OverlayDecoder(OverlayDecoderPrivate *mvcdec);
	};


	/** @brief Subtitle overlay renderer.
	  * Description
	  * This class is a special overlay decoder which renders
	  * CineCanvas™ and SMPTE 428-7 into the overlay buffer and
	  * handles the output timing.                             
	*/
	class MVC2_API SubtitleDecoder : public MvcDecoder
	{
	public:
		enum RENDERFLAGS
		{
			Render_Force_Shadow			= (1<<0),
			Render_Force_Border			= (1<<1),
			Render_Soft_Shadows			= (1<<2),
			Render_3D_Mode					= (1<<3),
			Render_BaseResolution_2K	= (0<<4),
			Render_BaseResolution_4K	= (1<<4),
			Render_2D_Left_Shift			= (1<<5),
			Render_2D_Right_Shift		= (2<<5),
		};
	public:
		virtual ~SubtitleDecoder();

		SubtitleDecoder();
		SubtitleDecoder(TMmRc * resultPointer, const MvcDevice &mvcdev);
		SubtitleDecoder(const SubtitleDecoder& other);
		SubtitleDecoder& operator= (const SubtitleDecoder& other);

		/* @brief Connects an output object to the decoder.
		Description
		This method is used to assign an output to the decoder which
		is used to display the decoder frames.
		@param VideoOut  dedicated VideoOutput object.
		@return MMRC_Ok or an error if the connection fails.
		@retval MMRC_Ok                    \Output successfully
		connected
		@retval MMRC_MVC2_ConnectionError  \Output could not be
		connected, it may not be
		compatible                */
		TMmRc connectOutput(const VideoOutput& VideoOut);

		/* @brief Disconnects the output.
		Description
		Disconnects a previously connected output from the decoder,
		so it can be reused for another decoder.
		@param VideoOut  previously connected output
		@return MMRC_Ok if the disconnection was successful, else an
		error.
		@retval MMRC_Ok                    successful disconnections
		@retval MMRC_MVC2_ConnectionError  object is not connected to
		the decoder                */
		TMmRc disconnectOutput(const VideoOutput& VideoOut);

		virtual TMmRc getDataBuffer(OverlayElementDataBuffer &databuf, uint32_t minsize = 0, uint32_t timeout = WaitInfinite);
		virtual TMmRc getDataBuffer(OverlaySubtitleDataBuffer &databuf, uint32_t minsize = 0, uint32_t timeout = WaitInfinite);

		TMmRc getOverlayElementRequest(OverlayElementDataBuffer &databuf, uint32_t timeout = WaitInfinite);

		TMmRc sendOverlayElement(const char *elementName, const uint8_t *data, uint32_t dataSize, uint32_t resourceId, 
			uint16_t keyIndex = DataBuffer::NoDecryption, uint8_t *iv = 0, uint8_t *cv = 0, uint32_t plainOffset = 0, uint32_t sourceLength = 0);

		TMmRc sendSubtitleFile(const uint8_t *data, uint32_t dataSize, uint32_t *resourceId = 0, int32_t timeOffset = 0, uint32_t startTime = 0, uint32_t endTime = UINT32_MAX);

		TMmRc enableSubtitles(int32_t localTimeOffset = 0, uint32_t flags = 0);
		TMmRc disableSubtitles();

		TMmRc setZOffset(float offset);

		TMmRc setOutputResolution(uint32_t width, uint32_t height, uint32_t flags = OverlayDecoder::Resolution_Absolute, uint32_t resourceId = 0);

	private:
		explicit SubtitleDecoder(SubtitleDecoderPrivate *mvcdec);
	};

}
#endif
