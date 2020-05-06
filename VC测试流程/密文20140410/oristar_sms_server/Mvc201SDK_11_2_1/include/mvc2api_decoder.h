#ifndef _MVC2API_DECODER_H_
#define _MVC2API_DECODER_H_

#include "mvc2api_types.h"
#include "mvc2api_device.h"
#include "mvc2api_output.h"
#include "mvc2api_securitymanager.h"

namespace mvc2
{
	class MvcDecoderPrivate;
	class MemoryBuffer;
	class MvcDecoder;
	class BufferStatusPrivate;
	class DecoderStatusPrivate;
	class DecoderErrorsPrivate;
	class ActiveDecoderIteratorPrivate;
	class ActiveDecoderInfoPrivate;
	class Dolby3Dchromaticity;
	class Dolby3DchromaticityPrivate;

	/** @brief TimeCode value type.
	  * Description
	  * Objects of this call hold a time code and make manipulations
	  * very easy.                                                  
	*/
	class	MVC2_API TimeCode
	{
	public:
		/** @brief TimeCode default constructor.
		  * Description
		  * A object with time code 00:00:00.000 without drop frame will
		  * be created.                                                 
		*/
		TimeCode();
		/** @brief Constructs a TimeCode with a given value.
		  * Description
		  * A new TimeCode object will be created with the value of
		  * packettc. See <link mvc2::TimeCode::getTimeCodePacked@const, getTimeCodePacked()>.
		  * @param packedtc  Time code represented in a 32 bit integer.                       
		*/
		TimeCode(uint32_t packedtc);
		/** @brief Copy constructor.
		  * Description
		  * This constructor creates a new object with the value of a
		  * source object.
		  * @param other  TimeCode object to copy the value from.    
		*/
		TimeCode(const TimeCode &other);
		/** @brief Constructs a TimeCode object with a given value.
		  * Description
		  * A new TimeCode object will be created and initialized.
		  * @param hours      hours (0\-23)
		  * @param mins       minutes (0\-59)
		  * @param secs       seconds (0\-59)
		  * @param frames     frames (0\-255)
		  * @param dropframe  drop frame flag                      
		*/
		TimeCode(uint8_t hours, uint8_t mins, uint8_t secs, uint8_t frames, bool dropframe);

		/** @brief Gets hours info.
		  * Description
		  * This method returns the hours value from the TimeCode.
		  * @return 8 bit hours value.<p />                       
		*/
		uint8_t getHours() const;
		/** @brief Gets minutes info.
		  * Description
		  * This method returns the minutes value from the TimeCode.
		  * @return 8 bit minutes value.<p />                       
		*/
		uint8_t getMinutes() const;
		/** @brief Gets seconds info.
		  * Description
		  * This method returns the seconds value from the TimeCode.
		  * @return 8 bit seconds value.<p />                       
		*/
		uint8_t getSeconds() const;
		/** @brief Gets frames info.
		  * Description
		  * This method returns the frames value from the TimeCode.
		  * @return 8 bit frames value.<p />                       
		*/
		uint8_t getFrames() const;
		/** @brief Gets drop frame flag.
		  * Description
		  * This method is used to check if drop frame flag is set.
		  * @return True if drop frame flag is set, otherwise false.<p />
		*/
		bool isDropFrame() const;

		/** @brief Gets the time code in packed format.
		  * Description
		  * This method returns the time code in a 32 bit representation.
		  * <table>
		  * bits    \description
		  * ------  ----------------
		  * 7-0     frames
		  * 15-8    seconds
		  * 23-16   minutes
		  * 30-24   hours
		  * 31      drop frame flag
		  * </table>
		  * @return A 32 bit value with the hold time code information.<p />
		*/
		uint32_t getTimeCodePacked() const;

		/** @brief Assignment operator.
		  * Description
		  * This operator copies one TimeCode over another.
		  * @param other  source TimeCode object.
		  * @return A reference to the destination TimeCode object.<p />
		*/
		TimeCode &operator =(const TimeCode &other);

	private:
		uint32_t m_timecode;
	};



	/* \ \ 
	@brief Transfer data buffer class.
	Description
	This class is used to transfer data to the MVC card. A
	decoder will create a number of DataBuffers which will be
	provided by the decoder. A application should get a
	DataBuffer from the decoder, fill it with one frame and set
	additional information (like timestamp or userdata). After
	the data was copied to the buffer, the application will set
	the size to transfer using <link mvc2::DataBuffer::setInSize@uint32_t, setInSize()>
	method. The transfer will be started by the <link mvc2::DataBuffer::send, send()>
	method. After calling <link mvc2::DataBuffer::send, send()>,
	the buffer is invalid an cannot be used anymore.                                    */
	class MVC2_API DataBuffer
	{
	public:
		/* \ \ 
		Description
		This static identifier specifies that the data is not
		decrypted.                                            */
		static const uint16_t NoDecryption = 0xffff;

	public:
		DataBuffer();
		/* @brief Destructor
		   Description
		   Default destructor of a DataBuffer. All references to memory
		   will be removed.                                             */
		virtual ~DataBuffer();

		/* @brief Send buffer to MVC card.
		Description
		After the buffer was filled, the data can be written to the
		MVC card. The send method will transfer the buffer to the
		driver. The number of bytes which will be transferred must be
		specified by the application before calling <link mvc2::DataBuffer::send, send()>
		using <link mvc2::DataBuffer::setInSize@uint32_t, setInSize()>
		method. After calling send(), the DataBuffer is empty, it
		doesn't hold a memory space anymore.                                               */
		TMmRc send();

		/* @brief Send DataBuffer to the MVC card.
		Description
		Same method as <link mvc2::DataBuffer::send, send()>, but the
		transfer size will be set before.
		@param size  number of bytes to transfer                      */
		TMmRc send(uint32_t size);

		/* @brief Wait for transfer completion
		Description
		After <link mvc2::DataBuffer::send, send()> method was
		called, this method could be used to wait for the transfer
		completion of this buffer. Normally this method is not needed
		for transfers, because the number of buffers is limited, so
		the API will automatically wait for a free buffer from time
		to time.
		@param timeout  number of milliseconds to wait for the
		transfer completion
		@return TMmRc of the operation
		@retval MMRC_Timeout  operation timed out
		@retval MMRC_Ok				transfer completed                 */
		TMmRc wait(uint32_t timeout);

		/* @brief Copy a data block to DataBuffer.
		Description
		This method will copy data to the DataBuffer object and
		adjusts the size. It's more easy to use the <link mvc2::DataBuffer::getBufferAddress, getBufferAddress()>
		\+ memcpy() +<link mvc2::DataBuffer::setInSize, setInSize()>.
		@param srcblock  pointer to a array of bytes to copy from
		@param size      number of bytes to copy
		@return number of bytes which were copied
		*/
		uint32_t copy(const uint8_t *srcblock, uint32_t size);

		/* @brief Get the available buffer size.
		Description
		This method returns the number of bytes available in this
		DataBuffer. <link mvc2::DataBuffer::getInSize, setInSize()>
		should be used to set the data size which needs to be
		transferred to the MVC card.
		@return Number of bytes available in this buffer.           */
		uint32_t getFreeSize();

		/* @brief Get the transfer size.
		Description
		This method will return the size which the application has
		selected for the transfer. It's the size which was set using
		the <link mvc2::DataBuffer::getInSize, setInSize()> method.
		@return Number of bytes to transfer to the card.             */
		uint32_t getInSize();

		/* @brief Sets the transfer size.
		Description
		After the data buffer was filled, the application needs to
		tell how many bytes are in the buffer and need to be
		transferred to the MVC card. The default size is zero, so no
		data will be transferred by default.
		@param size  number of bytes to transfer to the MVC card     */
		void setInSize(uint32_t size);

		/* @brief Set decryption size.
		Description
		A data transfer is split into three parts. First a number of unencrypted
		bytes following the encrypted data. The encrypted data itself is stuff to
		be dividable by 16. 
		@param plainOffset  number of plain bytes at the start of the data block
		@param sourceLength  total number of bytes after decryption (plainOffset + 
		encryptionSize - encryptionStuffing)                         */
		void setDecryptionSize(uint32_t plainOffset, uint32_t sourceLength);

		/* @brief Gets the buffer memory address.
		Description
		This method will return the memory address of the DataBuffer.
		Do not copy more data into this buffer then the maximal size.
		The buffer size will be return by <link mvc2::DataBuffer::getFreeSize, getFreeSize()>
		method.
		@return Memory address or NULL if the buffer doesn't hold any
		memory.                                                                               */
		uint8_t *getBufferAddress();

		/* @brief Sets the presentation time stamp.
		Description
		This method will set the presentation time stamp for the
		frame inside of the DataBuffer. The time stamp will start at
		0 for the first frame and will count in 90kHz ticks.
		@param stamp  presentation time stamp in 90kHz ticks         */
		void setTimeStamp(uint32_t stamp);

		/* @brief Get current timestamp of the DataBuffer.
		   Description
		   Some decoder may calculate the time stamp for a new
		   DataBuffer automatically (e.g. by setting a frame rate). This
		   function will return the current time stamp. It's the
		   calculated time stamp for a new DataBuffer or the stamp set
		   using <link mvc2::DataBuffer::setTimeStamp@uint32_t, setTimeStamp()>.
		   @return Returns the time stamp of the DataBuffer which is
		   based on 90kHz ticks.
		                                                                         */
		uint32_t getTimeStamp() const;

		/* @brief Sets user data.
		Description
		Every frame holds a user data field which can be read out if
		the frame is displayed. The usage of this field is up to the
		application.
		@param data  32 bit user data                                */
		void setUserData(uint32_t data);

		/* @brief Set decryption information.
		   Description
		   This method will set all necessary information needed to
		   decrypt the data. The decryption will only be enabled if all
		   fields are filled with valid data and the plain size is
		   smaller then the size of the DataBuffer.
		   @param keyindex  index of the AES key for the decryption or
		                    NO_DECRYPTION for plain data.
		   @param iv        128 bit (16 bytes array) initialization
		                    vector for the AES decryption
		   @param cv        128 bit (16 bytes array) check value for the
		                    AES decryption                               */
		TMmRc setKeyIndex(uint16_t keyindex, const uint8_t *iv, const uint8_t *cv);

		/** @brief Set decryption information.
		  * Description
		  * This method will set all necessary information needed to
		  * decrypt the data. The decryption will only be enabled if all
		  * fields are filled with valid data and the plain size is
		  * smaller then the size of the DataBuffer. This function will
		  * use the security manager if available and set to the decoder
		  * to translate the keyid to the keyindex. The security manager
		  * must have the CPL and KDM and playShow() must already been
		  * called.
		  * @param keyid  Key UUID for decryption. 
		  * @param kdmid  KDM Message UUID to use for decryption. (optional, will use CplUid lookup if not specified) 
		  * @param iv     128 bit (16 bytes array) initialization vector
		  *               for the AES decryption 
		  * @param cv     128 bit (16 bytes array) check value for the AES
		  *               decryption                                      
		*/
		TMmRc setKeyId(const mvc2::UuidValue &keyid, const uint8_t *iv, const uint8_t *cv);
		TMmRc setKeyId(const mvc2::UuidValue &keyid, const mvc2::UuidValue &kdmid, const uint8_t *iv, const uint8_t *cv);

		/** @brief Set the MIC value extracted from the MXF file.
		  * Description
		  * The function sets the MIC which will be used for integrity
		  * checks. It starts with the first byte of the BER of the track
		  * UUID and ends with the last byte of the MIC value.
		  * @param micBlock  pointer to the MIC block of the MXF
		  * @param size      size of the micBlock in bytes
		  * @return Returns MMRC_Ok if internal save went well.<p />     
		*/
		TMmRc setMicValue(const uint8_t *micBlock, uint32_t size);

		/* @brief Assignment operator.
		Description
		This operator copies the information from one DataBuffer to
		another.
		@param inbuf  Source DataBuffer
		@return If the source DataBuffer is not empty, a true will be
		returned, otherwise false.
		*/
		DataBuffer &operator = (const DataBuffer &other);

      DataBuffer(const DataBuffer& other);

      operator bool() const;

	protected:
		DataBuffer(MvcDecoderPrivate *mvcdec, MemoryBuffer *mem);

	protected:
		/* pointer to internal memory buffer object */
		MemoryBuffer *m_memBuffer;
		/* pointer to private decoder object */
		MvcDecoderPrivate *m_decoderPrivate;

		friend class MvcDecoder;
	};

	class MVC2_API VideoDataBuffer : public DataBuffer
	{
	public:
		VideoDataBuffer();
		virtual ~VideoDataBuffer();

		void setTimeCode(const TimeCode &tc);

	protected:
		VideoDataBuffer(MvcDecoderPrivate *mvcdec, MemoryBuffer *mem);

		friend class VideoDecoder;
	};

	class MVC2_API PCMDataBuffer : public DataBuffer
	{
	public:
		PCMDataBuffer();
		virtual ~PCMDataBuffer();

		TMmRc setChannelMapping(const uint8_t *mapping, uint32_t mappingSize);

		TMmRc setSampleFrequency(uint32_t frequency);

		TMmRc setNumberOfChannels(uint8_t numberOfchannels);

		TMmRc setBitsPerSample(uint8_t bitsPerSample);

		TMmRc setMixingChannel(uint8_t channel, const float *mixingValues, uint32_t mixingSize);

	protected:
		PCMDataBuffer(MvcDecoderPrivate *mvcdec, MemoryBuffer *mem);

		friend class PCMDecoder;
	};

	class MVC2_API BufferStatus
	{
	public:
		enum INTERVAL
		{
			Interval_100ms		= 0,
			Interval_1s			= 1,
			Interval_10s		= 2
		};

	public:
		BufferStatus(const MvcDevice &mvcdev, uint32_t channel);
		BufferStatus(MvcDecoderPrivate *mvcdec);
		BufferStatus(const BufferStatus &other);

		~BufferStatus();

		uint32_t getDecoderBufferSize() const;

		uint32_t getDecoderBufferMax() const;

		float getDecoderFullness() const;

		uint32_t getPendingFrames() const;

		uint32_t getFreeDataBuffers() const;

		uint32_t getBitrateDriver(INTERVAL interval = Interval_1s) const;

		uint32_t getBitrateDecoder(INTERVAL interval = Interval_1s) const;

		BufferStatus &operator =(const BufferStatus &inbuf);

		operator bool() const;

		TMmRc getReturnCode() const;

	private:
		BufferStatusPrivate *m_StatusPriv;
	};

	class MVC2_API DecoderStatus
	{
	public:
		DecoderStatus(const MvcDevice &mvcdev, uint32_t channel);
		DecoderStatus(MvcDecoderPrivate *mvcdec);
		DecoderStatus(MvcDecoderPrivate *mvcdec, const DecoderStatus &other);
		DecoderStatus(const DecoderStatus &other);
		DecoderStatus(const int32_t *values, uint32_t size);
		DecoderStatus();

      virtual ~DecoderStatus();

		uint32_t getTimeStamp() const;

		uint32_t getUserData() const;

		TMmRc getReturnCode() const;

		/* @brief Returns if the object is valid.
		Description
		\Returns true if the object is valid or false if the object
		is invalid.                                                 */
		operator bool() const;

		DecoderStatus &operator =(const DecoderStatus &other);

	protected:
		DecoderStatusPrivate *m_StatusPriv;
	};

	class MVC2_API VideoDecoderStatus : public DecoderStatus
	{
	public:
		VideoDecoderStatus(const MvcDevice &mvcdev, uint32_t channel);
		VideoDecoderStatus(MvcDecoderPrivate *mvcdec);
		VideoDecoderStatus(MvcDecoderPrivate *mvcdec, const VideoDecoderStatus &other);
		VideoDecoderStatus(const VideoDecoderStatus &other);
		VideoDecoderStatus();

		uint16_t getPictureWidth() const;

		uint16_t getPictureHeight() const;

		TimeCode getTimeCode();

		VideoDecoderStatus &operator =(const VideoDecoderStatus &instat);
	};

	class MVC2_API AudioDecoderStatus : public DecoderStatus
	{
	public:
		enum INTERVAL
		{
			Interval_50ms		= 0,
			Interval_1s			= 1,
		};

	public:
		AudioDecoderStatus(const MvcDevice &mvcdev, uint32_t channel);
		AudioDecoderStatus(MvcDecoderPrivate *mvcdec);
		AudioDecoderStatus(MvcDecoderPrivate *mvcdec, const AudioDecoderStatus &other);
		AudioDecoderStatus(const AudioDecoderStatus &other);
		AudioDecoderStatus();

		uint32_t getAudioLevel(uint32_t audioChannel, INTERVAL interval = Interval_50ms) const;
		bool isAudioChannelActive(uint32_t audioChannel) const;

		AudioDecoderStatus &operator =(const AudioDecoderStatus &instat);
	};

	class MVC2_API DecoderErrors
	{
	public:
		DecoderErrors(MvcDecoderPrivate *mvcdec);
		DecoderErrors(const MvcDevice &mvcdev, uint32_t channel);
		DecoderErrors(const DecoderErrors &other);
		DecoderErrors();

		virtual ~DecoderErrors();

		DecoderErrors &operator =(const DecoderErrors &other);

		/* @brief Returns if the object is valid.
		Description
		\Returns true if the object is valid or false if the object
		is invalid.                                                 */
		operator bool() const;

		TMmRc getReturnCode() const;

	protected:
		DecoderErrorsPrivate *m_ErrorsPriv;
	};


	class MVC2_API VideoDecoderErrors : public DecoderErrors
	{
	public:
		VideoDecoderErrors(MvcDecoderPrivate *mvcdec);
		VideoDecoderErrors(const VideoDecoderErrors &other);
		VideoDecoderErrors(const MvcDevice &mvcdev, uint32_t channel);
		VideoDecoderErrors();

		virtual ~VideoDecoderErrors()
		{
		}

		VideoDecoderErrors &operator =(const VideoDecoderErrors &other);

		uint32_t getPlayedFramesOk() const;

		uint32_t getPlayedFramesError() const;

		uint32_t getSkippedFramesAVSync() const;

		uint32_t getSkippedFramesError() const;

		uint32_t getRepeatedFramesAVSync() const;

		uint32_t getRepeatedFramesUnderrun() const;

		uint32_t getInputUnderruns() const;

		uint32_t getPartialFrames() const;
	};

	class MVC2_API AudioDecoderErrors : public DecoderErrors
	{
	public:
		AudioDecoderErrors(MvcDecoderPrivate *mvcdec);
		AudioDecoderErrors(const AudioDecoderErrors &other);
		AudioDecoderErrors(const MvcDevice &mvcdev, uint32_t channel);
		AudioDecoderErrors();

		virtual ~AudioDecoderErrors()
		{
		}

		AudioDecoderErrors &operator =(const AudioDecoderErrors &other);

		uint32_t getPlayedSamples(uint32_t audioChannel) const;
		uint32_t getSkippedSamples(uint32_t audioChannel) const;
		uint32_t getSilentSamples(uint32_t audioChannel) const;
		uint32_t getInputUnderruns(uint32_t audioChannel) const;

		uint32_t getClippedSamples(uint32_t audioChannel) const;
		uint32_t getNumberOfClippings(uint32_t audioChannel) const;
		uint32_t getMaxClippingPeriode(uint32_t audioChannel) const;
	};



	/* This is the base class of all decoders of a MVC card. It
	   handles the basic functions like data transfers and
	   end-of-stream.                                           */
	class MVC2_API MvcDecoder
	{
		friend class PlaybackControl;

	public:
		enum TIMEOUT
		{
			WaitInfinite = 0xffffffff, /* Wait forever. */
			DonotWait = 0 /* Do not wait. Return function immediately. */
		};

		enum DecoderType
		{
			Type_UnknownDecoder			= 0,
			Type_Jpeg2kDecoder			= 1,
			Type_PCMDecoder				= 2,
			Type_Mpeg2Decoder				= 3,
			Type_TestDecoder				= 4,
			Type_OverlayDecoder			= 5,
			Type_SubtitleDecoder			= 6
		};

	public:
      MvcDecoder();

		virtual ~MvcDecoder();

		/* @brief Return a buffer for data transfer.
		Description
		This method will return a <link mvc2::DataBuffer, DataBuffer>
		which is used to transfer a frame to the MVC card. A <link mvc2::MvcDevice, MvcDevice>
		has a number of <link mvc2::DataBuffer, DataBuffer>s for the
		transfer. If all buffers are in use, the function can wait
		for the next empty buffer. The number of buffer available and
		the buffer size depend on the decoder, e.g. audio decoders
		will have smaller buffer because of the smaller frame sizes.
		@param databuf  A DataBuffer object which will be overwritten
		with the new DataBuffer
		@param minsize  minimum number of bytes which are needed for
		the transfer, the method will try to take care
		of this size, but it's not guaranteed that the
		buffer has this size
		@param timeout  number of milliseconds to wait for a available
		DataBuffer
		@return MMRC_Ok if the method call was successful, otherwise
		an error.
		@retval MMRC_Ok  operation successfully completed                                      */
		virtual TMmRc getDataBuffer(DataBuffer &databuf, uint32_t minsize = 0, uint32_t timeout = WaitInfinite);

		/* @brief Go to end of stream state.
		   Description
		   After all data has been transferred, the decoder should be
		   set to end-of-stream state. This ensures that even the last
		   frame will be displayed and it will automatically go to stop
		   state if the last frame was displayed. No more data can be
		   transferred if a decoder is in end-of-stream state.          */
		void setEndOfStream();

		/* @brief Gets the used channel number.
		Description
		Every decoder allocates a virtual transfer channel. To
		identify this channel an id will be assigned which this
		method returns.
		@return Transfer channel id.
		*/
      uint32_t getChannel() const;

		/* @brief Setup start delay for the decoder.
		   Description
		   The playback will start at a time stamp of zero, setting the
		   time stamp of the first frame to a different value will delay
		   the start by the given time. This method does simply the same
		   as setting the time stamp of the first frame.
		   @param delay  start delay in milliseconds                     */
		void setStartDelay(float delay);

      MvcDecoder(const MvcDecoder& other);
		MvcDecoder &operator =(const MvcDecoder& other);

		BufferStatus getBufferStatus();

		TMmRc getDecoderStatus(DecoderStatus &status) const;

		TMmRc waitDecoderStatus(DecoderStatus &status) const;

		TMmRc getDecoderErrors(DecoderErrors &errors) const;

		TMmRc waitForUserData(uint32_t userdataUpper, uint32_t userdataLower, uint32_t *userdata = 0);
		TMmRc waitForTimeStamp(uint32_t timestampUpper, uint32_t timestampLower, uint32_t *timestamp = 0);

		TMmRc waitForTransferFinish(uint32_t timeout = WaitInfinite);

		TMmRc setCplUid(const uint8_t *uid);

		TMmRc flush(uint32_t keepTime, uint32_t *nextTimeStamp);

		TMmRc setBackwardPlayback(bool backwards);

		DecoderType getDecoderType() const;

		TMmRc setSecurityManager(const SecurityManager &sm);

				/* @brief Sets the frame rate.
		   Description
		   The decoder has the ability to automatically calculate the
		   time stamps of the <link mvc2::DataBuffer, DataBuffers> if it
		   has the information about the frame rate. It will then
		   calculate the new time stamp if <link mvc2::MvcDecoder::getDataBuffer@DataBuffer &amp;@uint32_t@uint32_t, getDataBuffer()>
		   is called based on the time stamp of the last <link mvc2::DataBuffer::send, DataBuffer::send()>
		   call. Internally the frame rate and time stamp calculation is
		   done using 27 MHz ticks, which means there will be done some
		   rounding of the frame rate value. The methods <link mvc2::Jpeg2kDecoder::getFrameRate, getFrameRate()>
		   and <link mvc2::Jpeg2kDecoder::getFrameRateTicks, getFrameRateTicks()>
		   should be used to verify the used frame rate.
		   @param rate  frame rate in frames per second                                                                               */
		void setFrameRate(float rate);

		/* @brief Gets the frame rate.
		   Description
		   This method returns the current frame rate based on frames
		   per second.
		   @return Frames per second or zero of the frame rate was
		   setup.
		                                                              */
		float getFrameRate() const;

		/* @brief Sets the frame rate in ticks.
		   Description
		   The decoder has the ability to automatically calculate the
		   time stamps of the <link mvc2::DataBuffer, DataBuffers> if it
		   has the information about the frame rate. It will then
		   calculate the new time stamp if <link mvc2::MvcDecoder::getDataBuffer@DataBuffer &amp;@uint32_t@uint32_t, getDataBuffer()>
		   is called based on the time stamp of the last <link mvc2::DataBuffer::send, DataBuffer::send()>
		   call.
		   @param ticks  Frame rate in 27 MHz ticks (e.g. 1125000 means 24
		                 frames per second)                                                                                           */
		void setFrameRateTicks(uint32_t ticks);

		/* @brief Gets frame rate ticks.
		   Description
		   This method returns the current frame rate based on 27 MHz
		   ticks. For example 27000000 means 10 frames per second.
		   @return Frame rate in 27 MHz ticks or zero if no frame rate
		   was setup.
		                                                               */
		uint32_t getFrameRateTicks() const;


		/* @brief Returns if the object is valid.
         Description
         \Returns true if the object is valid or false if the object
         is invalid.                                                 */
      operator bool() const;

   protected:
      MvcDecoderPrivate *getPrivate() const;

      MvcDecoder(MvcDecoderPrivate *mvcdec);

	   MvcDecoderPrivate *m_DecoderPrivate;
	};

	class MVC2_API VideoDecoder : public MvcDecoder
	{
	public:
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

		TMmRc getVideoDataBuffer(VideoDataBuffer &databuf, uint32_t minsize = 0, uint32_t timeout = WaitInfinite);

		TMmRc getDecoderStatus(VideoDecoderStatus &status) const;

		TMmRc waitDecoderStatus(VideoDecoderStatus &status) const;

		TMmRc getDecoderErrors(VideoDecoderErrors &errors) const;

		TMmRc setCloneVideo(bool clone);

		TMmRc setDolby3DMatrix(const float *matrix);

		TMmRc setDolby3DChroma(const Dolby3Dchromaticity &dolby3d);

		static TMmRc calcDolby3DMatrix(const Dolby3Dchromaticity &dolby3d, float *matrix);

	protected:
		VideoDecoder(MvcDecoderPrivate *mvcdec);
		VideoDecoder();

	};


   class Jpeg2kDecoderPrivate;

	/* \ \ 
	@brief JPEG 2000 decoder class.

	Description
	This class will create a JPEG 2000 video decoder on the MVC
	card and establish a virtual transfer channel.              */
	class MVC2_API Jpeg2kDecoder : public VideoDecoder
	{
	public:
		enum MODE_4K
		{
			MODE_4K_Off				= 0,				// only decode 2K resolution
			MODE_4K_Automatic		= 1,				// automatically switch to 4K depending on the video material
			MODE_4K_Always			= 2,				// upscale any resolution to 4K
         MODE_4K_DropScale    = 3            
		};

		enum CREATION_FLAGS
		{
			Create_4K_Decoder			= (1<<0),		// 4K decoding capability
			Create_HighFrameRate		= (1<<1),		// need support for high frame rates, beyond 60p 2D or 30p 3D
		};

		/* @brief Default destructor.
		Description
		The default destructor of the Jpeg2kDecoder will release the
		virtual transfer channel and all other resources allocated by
		the decoder. The decoder will also detached from the <link mvc2::PlaybackControl, PlaybackControl>. */
		virtual ~Jpeg2kDecoder();

      /* @brief Creates an empty (invalid) Jpeg 2000 Decoder object.
         Description
         Creates an empty (invalid) Jpeg 2000 Decoder object.        */
      Jpeg2kDecoder();
      /** @brief This method will create a JPEG 2000 decoder object.
        * Description
        * This method will create a JPEG 2000 decoder object and
        * allocate the virtual transfer channel for it.<p />
        * The decoder could be created the object will be valid the
        * resultPointers value is set to MMRC_Ok. If not the object is
        * set to an invalid state and the resultPointers value contains
        * the error code. The default decoder is a 2K decoder which can
        * decode up to 60p if running in 2D mode (not linked). Using
        * the createFlags a 4K and high frame rate decoder can created.
        * @param resultPointer  Pointer where to store the result in. if
        *                       0 no result will be written. 
        * @param mvcDevice      MvcDevice object to which the decoder
        *                       will be attached.
        * @param creationFlags  specify which capabilities this decoder
        *                       should have                             
      */
      Jpeg2kDecoder(TMmRc * resultPointer, const MvcDevice &mvcDevice, uint32_t creationFlags = 0);

		/** @brief Creates a linked Jpeg2k decoder for 3D playback.
		  * Description
		  * This constructor creates a secondary decoder to use with 3D
		  * playback.
		  * @param resultPointer  error code from the object creation
		  * @param linkedDecoder  Jpeg2k decoder to link the new decoder
		  *                       to                                    
		*/
		Jpeg2kDecoder(TMmRc * resultPointer, const Jpeg2kDecoder &linkedDecoder);

		Jpeg2kDecoder(const Jpeg2kDecoder& other);
      Jpeg2kDecoder& operator= (const Jpeg2kDecoder& other);


		/* @brief Sets the frame rate.
		   Description
		   The decoder has the ability to automatically calculate the
		   time stamps of the <link mvc2::DataBuffer, DataBuffers> if it
		   has the information about the frame rate. It will then
		   calculate the new time stamp if <link mvc2::MvcDecoder::getDataBuffer@DataBuffer &amp;@uint32_t@uint32_t, getDataBuffer()>
		   is called based on the time stamp of the last <link mvc2::DataBuffer::send, DataBuffer::send()>
		   call. Internally the frame rate and time stamp calculation is
		   done using 27 MHz ticks, which means there will be done some
		   rounding of the frame rate value. The methods <link mvc2::Jpeg2kDecoder::getFrameRate, getFrameRate()>
		   and <link mvc2::Jpeg2kDecoder::getFrameRateTicks, getFrameRateTicks()>
		   should be used to verify the used frame rate.
		   @param rate  frame rate in frames per second                                                                               */
		void setFrameRate(float rate);

		/* @brief Gets the frame rate.
		   Description
		   This method returns the current frame rate based on frames
		   per second.
		   @return Frames per second or zero of the frame rate was
		   setup.
		                                                              */
		float getFrameRate() const;

		/* @brief Sets the frame rate in ticks.
		   Description
		   The decoder has the ability to automatically calculate the
		   time stamps of the <link mvc2::DataBuffer, DataBuffers> if it
		   has the information about the frame rate. It will then
		   calculate the new time stamp if <link mvc2::MvcDecoder::getDataBuffer@DataBuffer &amp;@uint32_t@uint32_t, getDataBuffer()>
		   is called based on the time stamp of the last <link mvc2::DataBuffer::send, DataBuffer::send()>
		   call.
		   @param ticks  Frame rate in 27 MHz ticks (e.g. 1125000 means 24
		                 frames per second)                                                                                           */
		void setFrameRateTicks(uint32_t ticks);

		/* @brief Gets frame rate ticks.
		   Description
		   This method returns the current frame rate based on 27 MHz
		   ticks. For example 27000000 means 10 frames per second.
		   @return Frame rate in 27 MHz ticks or zero if no frame rate
		   was setup.
		                                                               */
		uint32_t getFrameRateTicks() const;

		TMmRc setColorSpace(const ColorSpace &space);

		TMmRc setCutOffLevel(uint8_t cutoff);
		TMmRc setResolutionDivider(uint8_t div);
		TMmRc set4kMode(MODE_4K mode);

	private:
		explicit Jpeg2kDecoder(Jpeg2kDecoderPrivate * mvcdec);

	};

   class PCMDecoderPrivate;


	/** The PCM decoder class does the pre-processing of PCM samples
	   before transferring it to an Audio-Output.                   */
	class MVC2_API PCMDecoder : public MvcDecoder
	{
	public:
		enum
		{
			ChannelMapping_UnusedChannel		= 0xff,
			ChannelMapping_MixedChannel		= 0x80
		} CHANNEL_MAPPING_FLAGS;
	public:
		virtual ~PCMDecoder();

      PCMDecoder();

      /** @brief Creates a PCMDecoder object.
         Description
         This method will create a PCMDecoder object if possible. The
         PCM decoder supports various sample formats and and a number
         of input channels. The sample data must be channel
         interleave, so it starts with the first sample of channel 0
         followed by the first sample of channel 1 and so on. If
         BitsPerChannel is 16 one sample consists of two bytes in
         little-endian order. If BitsPerSample is 17 to 24, a samples
         has 3 bytes in little-endian byte order.
         @param resultPointer Pointer where to store the result in. if 0
                              no result will be written.
         @param mvcDevice            MVC device where the decoder will be
         created
         @param bitsPerSample     bits per sample to process (possible
         values: 16 and 24)
         @param numberOfChannels  number of channels
         @param channelMask reserved
         @retval MMRC_Ok                          Decoder created
         successfully
         @retval MMRC_MVC2_OutOfDecoderResources  Decoder could not be
                 created, because there are no more resources available */
      PCMDecoder(TMmRc * resultPointer, const MvcDevice &mvcDevice, uint32_t bitsPerSample, 
                 uint32_t numberOfChannels, uint32_t channelMask = 0xffffffff);

      PCMDecoder(const PCMDecoder& other);
      PCMDecoder& operator= (const PCMDecoder& other);

		/* @brief Gets number of channels.
		   Description
		   This method returns the number of channels used by the
		   decoder. It's normally the value specified at create().
		   @return Number of channels.
		                                                           */
		uint32_t getNumberOfChannels() const;

		/* @brief Gets bits per sample.
		   Description
		   This method returns the number of bits per sample currently
		   in use.
		   @return Number of bits per sample.
		                                                               */
		uint32_t getBitsPerSample() const;

		/* @brief Connects the audio output.
		   Description
		   This method assigns a AudioOutput to the decoder which is
		   used to play the sample data to. The PCM data will be process
		   to fit the capabilities of the output.
		   @param AudioOut  AudioOutput object 
		   @return MMRC_Ok if the output was connected successfully,
		   else an error.
		   @retval MMRC_Ok                    Connection successful 
		   @retval MMRC_MVC2_ConnectionError  Connection could not be
		                                      established                */

		TMmRc connectOutput(const AudioOutput& audioOutput);
		/* @brief Disconnects the audio output.
		   Description
		   A previously connected output could be reused by another
		   decoder. Before this, it has to be disconnected.
		   @param audioOutput  Previously connected AudioOutput object 
		   @return MMRC_Ok if the disconnection was successful, else an
		   error.
		   @retval MMRC_Ok                    Disconnection successful 
		   @retval MMRC_MVC2_ConnectionError  an error occurred while
		                                      disconnecting             */
		TMmRc disconnectOutput(const AudioOutput& audioOutput);

		TMmRc getPCMDataBuffer(PCMDataBuffer &databuf, uint32_t minsize = 0, uint32_t timeout = WaitInfinite);

		TMmRc setSampleFrequency(uint32_t frequency);

		uint32_t getSampleFrequency() const;

		TMmRc getDecoderStatus(AudioDecoderStatus &status) const;

		TMmRc waitDecoderStatus(AudioDecoderStatus &status) const;

		TMmRc getDecoderErrors(AudioDecoderErrors &errors) const;

		TMmRc setChannelMapping(const uint8_t *mapping, uint32_t mappingSize);

		TMmRc setNumberOfChannels(uint8_t numberOfchannels);

		TMmRc setBitsPerSample(uint8_t bitsPerSample);

		TMmRc setMixingChannel(uint8_t channel, const float *mixingValues, uint32_t mixingSize);

		TMmRc setDolbyPrologicIIChannels(uint8_t destChannelLeft, uint8_t destChannelRight, uint8_t srcLeft, 
			uint8_t srcRight, uint8_t srcCenter, uint8_t srcSLeft, uint8_t srcSRight);


		/* @brief Sets the frame rate.
		   Description
		   The decoder has the ability to automatically calculate the
		   time stamps of the <link mvc2::DataBuffer, DataBuffers> if it
		   has the information about the frame rate. It will then
		   calculate the new time stamp if <link mvc2::MvcDecoder::getDataBuffer@DataBuffer &amp;@uint32_t@uint32_t, getDataBuffer()>
		   is called based on the time stamp of the last <link mvc2::DataBuffer::send, DataBuffer::send()>
		   call. Internally the frame rate and time stamp calculation is
		   done using 27 MHz ticks, which means there will be done some
		   rounding of the frame rate value. The methods <link mvc2::Jpeg2kDecoder::getFrameRate, getFrameRate()>
		   and <link mvc2::PCMDecoder::getFrameRateTicks, getFrameRateTicks()>
		   should be used to verify the used frame rate.
		   @param rate  frame rate in frames per second                                                                               */
		void setFrameRate(float rate);

		/* @brief Gets the frame rate.
		   Description
		   This method returns the current frame rate based on frames
		   per second.
		   @return Frames per second or zero of the frame rate was
		   setup.
		                                                              */
		float getFrameRate() const;

		/* @brief Sets the frame rate in ticks.
		   Description
		   The decoder has the ability to automatically calculate the
		   time stamps of the <link mvc2::DataBuffer, DataBuffers> if it
		   has the information about the frame rate. It will then
		   calculate the new time stamp if <link mvc2::MvcDecoder::getDataBuffer@DataBuffer &amp;@uint32_t@uint32_t, getDataBuffer()>
		   is called based on the time stamp of the last <link mvc2::DataBuffer::send, DataBuffer::send()>
		   call.
		   @param ticks  Frame rate in 27 MHz ticks (e.g. 1125000 means 24
		                 frames per second)                                                                                           */
		void setFrameRateTicks(uint32_t ticks);

		/* @brief Gets frame rate ticks.
		   Description
		   This method returns the current frame rate based on 27 MHz
		   ticks. For example 27000000 means 10 frames per second.
		   @return Frame rate in 27 MHz ticks or zero if no frame rate
		   was setup.
		                                                               */
		uint32_t getFrameRateTicks() const;
	private:
		explicit PCMDecoder(PCMDecoderPrivate *mvcdec);

	};


	class Mpeg2DecoderPrivate;

	/* \ \ 
	@brief MPEG-2 decoder class.


	Description
	This class will create a MPEG-2 video decoder on the MVC
	card and establish a virtual transfer channel.              */
	class MVC2_API Mpeg2Decoder : public VideoDecoder
	{
	public:
		enum PROCESSING_3D
		{
			Processing_None						= 0,
			Processing_SideBySide				= 1,
			Processing_TopBottom					= 2,
		};

	public:
		/* @brief Default destructor.
		Description
		The default destructor of the Mpeg2Decoder will release the
		virtual transfer channel and all other resources allocated by
		the decoder. The decoder will also detached from the <link mvc2::PlaybackControl, PlaybackControl>. */
		virtual ~Mpeg2Decoder();

		Mpeg2Decoder();

		/* @brief Creates a MPEG-2 decoder object.
		Description
		This method will create a MPEG-2 decoder object and
		allocate the virtual transfer channel for it.
		@param mvcdev  MvcDevice object to which the decoder will be
		attached 
		@param dec     pointer to a Mpeg2Decoder object which
		receives the newly created object 
		@return MMRC_Ok for a successful creation, otherwise an
		error.
		@retval MMRC_Ok               operation successfully completed
		@retval MMRC_MVC2_DeviceBusy  MvcDevice is already in use      */
		Mpeg2Decoder(TMmRc * resultPointer, const MvcDevice &mvcdev);

		Mpeg2Decoder(TMmRc * resultPointer, const Mpeg2Decoder &linkedDecoder);

		Mpeg2Decoder(const Mpeg2Decoder& other);
		Mpeg2Decoder& operator= (const Mpeg2Decoder& other);

		TMmRc set3DProcessing(PROCESSING_3D mode);

	private:
		explicit Mpeg2Decoder(Mpeg2DecoderPrivate *mvcdec);

	};

	class MVC2_API ActiveDecoderInfo
	{
	public:
		ActiveDecoderInfo(ActiveDecoderInfoPrivate *info = 0);
		ActiveDecoderInfo(const ActiveDecoderInfo &other);

		~ActiveDecoderInfo();

		ActiveDecoderInfo& operator= (const ActiveDecoderInfo& other);

		uint32_t getChannel() const;

	private:
		ActiveDecoderInfoPrivate *m_InfoPriv;
	};


	class MVC2_API ActiveDecoderIterator
	{
	public:
		ActiveDecoderIterator(TMmRc * resultPointer, const MvcDevice &mvcdev);

		ActiveDecoderIterator(const ActiveDecoderIterator &other);

		ActiveDecoderIterator& operator= (const ActiveDecoderIterator& other);

		~ActiveDecoderIterator();

		ActiveDecoderInfo getNext();

	private:
		ActiveDecoderIteratorPrivate *m_DecoderItorPriv;
	};


	class MVC2_API Dolby3Dchromaticity
	{
	public:
		enum Measurement
		{
			Chroma_red = 0,
			Chroma_green = 1,
			Chroma_blue = 2,
			Chroma_white = 4,
			Chroma_white_iter = 5,
			Chroma_red_proj = 6,
			Chroma_green_proj = 7,
			Chroma_blue_proj = 8,
			Chroma_white_proj = 9
		};

	public:
		Dolby3Dchromaticity();
		virtual ~Dolby3Dchromaticity();

		void setChroma(Measurement measurement, double x, double y);

		void getChroma(Measurement measurement, double &x, double &y) const;

		void setLuminance(double luma);

		double getLuminance() const;

		bool isWPointIterUsed() const;

		bool isCorrectionUsed() const;

	private:
		Dolby3DchromaticityPrivate *m_Private;
	};

}

#endif /* _MVC2API_DECODER_H_ */
