#ifndef _MVC2API_OUTPUT_H_
#define _MVC2API_OUTPUT_H_

#include "mvc2api_types.h"
#include "mvc2api_device.h"

namespace mvc2
{
	class VideoOutputPrivate;
	class AudioOutputPrivate;
	class AudioOutputStatusPrivate;
	class ExternalConnectorPrivate;

	class MVC2_API VideoMode
	{
	public:
		enum Mode
		{
			Mode_None								= 0,
			Mode_1920_1080_6000_p				= 1,
			Mode_1920_1080_5994_p				= 2,
			Mode_1920_1080_5000_p				= 3,
			Mode_1920_1080_4800_p				= 31,
			Mode_1920_1080_3000_p				= 4,
			Mode_1920_1080_2997_p				= 5,
			Mode_1920_1080_2500_p				= 6,
			Mode_1920_1080_6000_i				= 7,
			Mode_1920_1080_5994_i				= 8,
			Mode_1920_1080_5000_i				= 9,
			Mode_1920_1080_5000_i_1250			= 10,
			Mode_1920_1080_2400_p				= 11,
			Mode_1920_1080_2398_p				= 12,
			Mode_1920_1080_2400_psf				= 13,
			Mode_1920_1080_2398_psf				= 14,
			Mode_1920_1080_2400_psf_1250		= 15,
			Mode_1920_1080_2398_psf_1250		= 16,

			Mode_1280_720_6000_p					= 17,
			Mode_1280_720_5994_p					= 18,
			Mode_1280_720_5000_p					= 19,
			Mode_1280_720_3000_p					= 20,
			Mode_1280_720_2997_p					= 21,
			Mode_1280_720_2500_p					= 22,
			Mode_1280_720_2400_p					= 23,
			Mode_1280_720_2398_p					= 24,

			Mode_720_576_5000_i					= 25,
			Mode_720_576_2500_p					= 26,
			Mode_720_576_5000_p					= 27,
			Mode_720_480_5994_i					= 28,
			Mode_720_480_2997_p					= 29,
			Mode_720_480_5994_p					= 30,

			Mode_2048_1080_6000_p				= 32,
			Mode_2048_1080_5994_p				= 33,
			Mode_2048_1080_5000_p				= 34,
			Mode_2048_1080_4800_p				= 35,
			Mode_2048_1080_3000_p				= 36,
			Mode_2048_1080_2997_p				= 37,
			Mode_2048_1080_2500_p				= 38,
			Mode_2048_1080_2400_p				= 39,
			Mode_2048_1080_2398_p				= 40,
			Mode_2048_1080_6000_i				= 41,
			Mode_2048_1080_5994_i				= 42,
			Mode_2048_1080_5000_i				= 43,
			Mode_2048_1080_2400_psf				= 44,
			Mode_2048_1080_2398_psf				= 45,

			Mode_4096_2160_3000_p				= 50,
			Mode_4096_2160_2997_p				= 51,
			Mode_4096_2160_2500_p				= 52,
			Mode_4096_2160_2400_p				= 53,
			Mode_4096_2160_2398_p				= 54,

			Mode_1920_1080_12000_p				= 55,
			Mode_1920_1080_11988_p				= 56,
			Mode_1920_1080_10000_p				= 57,
			Mode_1920_1080_9600_p				= 58,

			Mode_2048_1080_12000_p				= 59,
			Mode_2048_1080_11988_p				= 60,
			Mode_2048_1080_10000_p				= 61,
			Mode_2048_1080_9600_p				= 62,

			Mode_Max
		};

	public:
		VideoMode(VideoMode::Mode mode = Mode_None) : m_mode(mode)
		{
		}

		VideoMode &operator =(const VideoMode &mode)
		{
			m_mode = mode.m_mode;
			return(*this);
		}

		/** @brief Gets video mode value.
		  * Description
		  * This function returns the actual <link mvc2::VideoMode::Mode, VideoMode::Mode>
		  * value from the object.
		  * @return <link mvc2::VideoMode::Mode, VideoMode::Mode> or
		  * VideoMode::Mode_None on failure.<p />                                         
		*/
		VideoMode::Mode getMode() const
		{
			return(m_mode);
		}

		uint32_t getWidth() const;
		uint32_t getHeight() const;
		double getFrameRate() const;
		bool isProgressive() const;

	private:
		VideoMode::Mode m_mode;
	};


	class MVC2_API ColorSpace
	{
	public:
		enum Space
		{
			Space_Default					= 0,
			Space_RGB						= 1,
			Space_YCbCr						= 2,
			Space_XYZ						= 3,
			Space_YCxCz						= 4,
			Space_YCbCr_P3					= 5,
			Space_Max
		};

		enum ColourPrimaries
		{
			Prim_Default					= 0,
			Prim_ITUR_BT_709				= 1,
			Prim_ITUR_BT_470_M			= 4,
			Prim_ITUR_BT_470_BG			= 5,
			Prim_SMPTE_170M				= 6,
			Prim_SMPTE_240M				= 7,
			Prim_P3							= 8,
			Prim_XYZ							= 100,
			Prim_sRGB						= 101
		};

		enum TransferCharacteristics
		{
			Trans_Default					= 0,
			Trans_ITUR_BT_709				= 1,
			Trans_ITUR_BT_470_M			= 4,
			Trans_ITUR_BT_470_BG			= 5,
			Trans_SMPTE_170M				= 6,
			Trans_SMPTE_240M				= 7,
			Trans_Linear					= 8,
			Trans_DCI						= 9,
			Trans_ITUR_BT_1361			= 10,
			Trans_sRGB						= 11
		};

		enum MatrixCoefficients
		{
			Coeff_Default					= 0,
			Coeff_ITUR_BT_709				= 1,
			Coeff_FCC						= 4,
			Coeff_ITUR_BT_470_BG			= 5,
			Coeff_SMPTE_170M				= 6,
			Coeff_SMPTE_240M				= 7,
			Coeff_DCI_P3					= 8,
			Coeff_YCxCz						= 20,
			Coeff_RGB						= 100,
			Coeff_XYZ						= 101
		};

	public:
		ColorSpace(Space spc = ColorSpace::Space_Default);
		ColorSpace(ColourPrimaries prim, TransferCharacteristics trans, MatrixCoefficients coeff);

		ColorSpace &operator =(const ColorSpace &spc);

		ColorSpace::Space getSpace() const;

		uint32_t getColorSpacePacked() const;

	private:
		uint8_t m_colour_primaries;
		uint8_t m_transfer_char;
		uint8_t m_matrix_coeff;
	};


	/** @brief Positioning class for an area.
	  * Description
	  * This class is used to specify a position of a rectangular
	  * area (e.g. video frame) on screen. It is used by <link mvc2::VideoOutput::setPosition@FramePosition &amp;@EFFECTSPEED@MOVEEFFECT, VideoOutput::setPosition()>.
	*/
	class MVC2_API FramePosition
	{
	public:
		/** Possible values for offset representation.
		*/
		enum MODE
		{
			Mode_Pixels						= (0<<0), /** Offset values are pixel values.
			*/
			
			Mode_Percent					= (1<<0), /** Offset values are percent values.
			*/
			
			Mode_Align_Center				= (0<<1), /** Center the frame inside the destination frame plus offset.
			*/
			
			Mode_Align_Left				= (1<<1), /** Align the left edge of the frame at the left edge of the
			  * destination frame plus x offset.                        
			*/
			
			Mode_Align_Top					= (1<<1),/** Align the top edge of the frame at the top edge of the
			  * destination frame plus y offset.                      
			*/
			
			Mode_Align_Right				= (2<<1), /** Align the right edge of the frame at the right edge of the
			  * destination frame plus x offset.                          
			*/
			
			Mode_Align_Bottom				= (2<<1)/** Align the bottom edge of the frame at the bottom edge of the
			  * destination frame plus y offset.                            
			*/
			
		};

	public:
		/** @brief Default constructor.
		  * Description
		  * The default constructor produces a FramePosition object for
		  * centering in horizontal and vertical direction.            
		*/
		FramePosition();
		/** @brief Copy constructor.
		  * Description
		  * This constructor creates a copy of a source FramePosition
		  * \object.
		  * @param other  source object to copy information from     
		*/
		FramePosition(const FramePosition &other);
		/** @brief Constructor to set a position.
		  * Description
		  * This constructor sets a integer based position.
		  * @param x       horizontal position offset as pixels or percent
		  * @param y       vertical position offset as pixels or percent 
		  * @param mode_x  a combination of FramePosition\:\:MODE bits to
		  *                specify the horizontal offset
		  * @param mode_y  a combination of FramePosition\:\:MODE bits to
		  *                specify the vertical offset                    
		*/
		FramePosition(int16_t x, int16_t y, uint16_t mode_x = (Mode_Pixels | Mode_Align_Center), uint16_t mode_y = (Mode_Pixels | Mode_Align_Center));

		/** @brief Gets x offset.
		  * @return Returns the current horizontal offset as integer. See
		  * <link mvc2::FramePosition::setX@int16_t, setX()> for details.<p />
		*/
		int16_t getX() const;
		/** @brief Gets y offset.<p />
		  * 
		  * @return Returns the current vertical offset as integer. See <link mvc2::FramePosition::setY@int16_t, setY()>
		  * for details.<p />                                                                                           
		*/
		int16_t getY() const;
		/** @brief Gets x offset as float value.
		  * @return Returns the horizontal offset as float point value.
		  * See <link mvc2::FramePosition::fsetX@float, FramePosition::fsetX()>
		  * for details.<p />                                                  
		*/
		float fgetX() const;
		/** @brief Gets y offset as float value.
		  * @return Returns the vertical offset as float point value. See
		  * <link mvc2::FramePosition::fsetY@float, FramePosition::fsetY()>
		  * for details.<p />                                              
		*/
		float fgetY() const;
		/** @brief Gets mode of x offset.
		  * @return Returns the current mode of horizontal offset. See <link mvc2::FramePosition::setModeX@uint16_t, setModeX()>
		  * and <link mvc2::FramePosition::MODE, FramePosition::MODE> for
		  * details.<p />                                                                                                       
		*/
		uint16_t getModeX() const;
		/** @brief Gets mode of y offset.
		  * @return Returns the current mode of vertical offset. See <link mvc2::FramePosition::setModeY@uint16_t, setModeY()>
		  * and <link mvc2::FramePosition::MODE, FramePosition::MODE> for
		  * details.<p />                                                                                                     
		*/
		uint16_t getModeY() const;
		/** @brief Sets x offset.
		  * Description
		  * Set the horizontal offset value as integer. The value
		  * represents a pixel offset or a percent value depending on <link mvc2::FramePosition::setModeX@uint16_t, ModeX>
		  * setup.
		  * @param x  integer offset in pixels or percent                                                                 
		*/
		void setX(int16_t x);
		/** @brief Sets y offset.
		  * Description
		  * Set the vertical offset value as integer. The value
		  * represents a pixel offset or a percent value depending on <link mvc2::FramePosition::setModeY@uint16_t, ModeY>
		  * setup.
		  * @param y  offset in pixels or percent                                                                         
		*/
		void setY(int16_t y);
		/** @brief Sets the x offset as float value.
		  * Description
		  * Offset, especially percent values can also have float values.
		  * All values are internally represented as fixed value, so the
		  * accuracy is not very high. Setting a float value
		  * automatically set the mode to <link mvc2::FramePosition::MODE::Mode_Percent, FrameMode::Mode_Percent>.
		  * @param x  float representation of a offset in percent                                                 
		*/
		void fsetX(float x);
		/** @brief Sets y offset as a float value.
		  * Description
		  * Offset, especially percent values can also have float values.
		  * All values are internally represented as fixed value, so the
		  * accuracy is not very high. Setting a float value
		  * automatically set the mode to <link mvc2::FramePosition::MODE::Mode_Percent, FrameMode::Mode_Percent>.
		  * @param y  float representation of a offset in percent                                                 
		*/
		void fsetY(float y);
		/** @brief Sets mode of x value.
		  * Description
		  * This method selects the way how the horizontal offset value x
		  * should be interpret.
		  * @param mode  a combination of FramePosition\:\:MODE bits     
		*/
		void setModeX(uint16_t mode);
		/** @brief Sets mode of y offset.
		  * Description
		  * This method selects the way how the vertical offset value x
		  * should be interpret.
		  * @param mode  a combination of FramePosition\:\:MODE bits   
		*/
		void setModeY(uint16_t mode);

		/** @brief Assignment operator.
		  * Description
		  * This operator copies all values from a source object to the
		  * current one.
		  * @param other  source object
		  * @return Returns a reference to the current object.<p />    
		*/
		FramePosition &operator =(const FramePosition &other);
		/** @brief Compares two objects.
		  * Description
		  * This operator compares a source object with the current one.
		  * @param other  source object
		  * @return Returns true if both objects are the same, otherwise
		  * false.<p />                                                 
		*/
		bool operator ==(const FramePosition &other) const;
		/** @brief Compares two objects.
		  * Description
		  * This operator compares a source object with the current one.
		  * @param other  source object
		  * @return Returns false of both objects are the same, otherwise
		  * true.<p />                                                   
		*/
		bool operator !=(const FramePosition &other) const;

	private:
		int16_t m_x;
		int16_t m_y;
		uint16_t m_mode_x;
		uint16_t m_mode_y;
	};

	class MVC2_API ColorConversionMatrix
	{
	public:
		ColorConversionMatrix(const float *matrix, const int32_t *post_add);

		ColorConversionMatrix &operator =(const ColorConversionMatrix &conv);

	private:
		float m_matrix[9];
		uint32_t m_post_add[3];
	};


	/* The VideoOutput class represents physical or virtual video
	   \outputs.                                                  */
	class MVC2_API VideoOutput
	{
      friend class Mpeg2Decoder;
      friend class Jpeg2kDecoder;
      friend class TestDecoder;
		friend class SecurityAccess;
		friend class OverlayDecoder;
		friend class VideoDecoder;
		friend class SubtitleDecoder;

	public:
		/* Possible properties of a video output. */
		enum VIDEOPROPERTY
		{
			VideoProperty_Default                  = 0,
			
			VideoProperty_SDTV							= (1<<0), /* \Output support standard definition TV frequencies. */
			
			VideoProperty_SDTV_Progressive			= (1<<1), /* \Output supports standard definition TV frequencies with
			   doubled frame rate as progressive. This includes 480p and
			   576p.                                                     */
			
			VideoProperty_HDTV							= (1<<2), /* \Output supports high definition TV frequencies. */
			
			VideoProperty_Dual_HDTV						= (1<<3),  /* \Output supports high definition TV frequencies with doubled
			   frame rate, like 1080p60.                                    */
			
			VideoProperty_VirtualLinked				= (1<<4) /* Linked outputs are only virtual and not physical available. */
		};

		/** Moving effect e.g. use by <link mvc2::VideoOutput::setPosition@FramePosition &amp;@EFFECTSPEED@MOVEEFFECT, setPosition()>.
		*/
		enum MOVEEFFECT
		{
			MoveEffect_Linear			=  0, /** The movement speed is linear from start to end.
			*/
			
			MoveEffect_SlowDown		=  1, /** The movement starts fast and slows down when it comes to end
			  * position.                                                   
			*/
			
			MoveEffect_SpeedUp		=  2, /** The movement starts slow and will speed up constantly.
			*/
			
		};

		/** Effect speed e.g. use by <link mvc2::VideoOutput::setPosition@FramePosition &amp;@EFFECTSPEED@MOVEEFFECT, setPosition()>.
		*/
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

		/** Possible values for <link mvc2::VideoOutput::setOutputActivity@OUTPUTACTIVITY@float, setOutputActivity()>.
		*/
		enum OUTPUTACTIVITY
		{
			Activity_Primary_Only						= 0, /** Only the primary video from the main VideoOutput is
			  * displayed.                                         
			*/
			
			Activity_Secondary_Only						= 1, /** Only the secondary video from the linked VideoOutput is
			  * displayed.                                             
			*/
			
			Activity_Primary_Secondary_as_Overlay	= 2, /** Secondary video is blended over the primary video.
			*/
			
			Activity_Secondary_Primary_as_Overlay	= 3, /** Primary video blended over secondary video.
			*/
			
			Activity_Primary_Secondary					= 4,/** Both videos are displayed, standard 3D mode without virtual
			  * VideoOutputs.                                              
			*/
			
		};

	public:
		/** @brief Default constructor for a VideoOutput.
		  * Description
		  * A empty VideoOutput object will be created.  
		*/
		VideoOutput();
		/* @brief Creates a VideoOutput object.
		   Description
		   This method creates a object for a physical or virtual video output.
		   Before the object will be created the list of available
		   \outputs will be parsed to find an output which fits best to
		   the VideoProperty.

         @param resultPointer  Pointer where to store the result in. if
         0 no result will be written. 
		   @param mvcdev         MvcDevice to create the output on 
		   @param VideoProperty  optional properties to influence the
		                         \output selection                                  */
		VideoOutput(TMmRc * resultPointer, const MvcDevice &mvcdev, uint32_t VideoProperty = 0);

		/** @brief Constructs a linked video output.
		  * Description
		  * A linked video output shares some parameter with another
		  * \output. Due to hardware limitations only one real physical
		  * video output can be created and all others have to be linked
		  * to it. Linked outputs share the same video mode, but can have
		  * different video decoder connected. They are mainly used to
		  * \output 3D videos. Second usage is to created virtual linked
		  * \outputs where the output video can be selected with <link mvc2::VideoOutput::setOutputActivity@OUTPUTACTIVITY@float, setOutputActivity()>
		  * \on the main VideoOutput.
		  * @param resultPointer  Return code of the creation, MMRC_Ok if
		  *                       anything was successful
		  * @param linked_output  reference to the main VideoOutput                                                                                   
		*/
		VideoOutput(TMmRc * resultPointer, const VideoOutput &linked_output);

		/** @brief VideoOutput copy constructor.
		  * Description
		  * Creates a new VideoOutput object from another VideoOutput.
		  * Both the old and the new VideoOutput object reference the
		  * same physical VideoOutput.
		  * @param vout  VideoOutput object to copy the reference from
		*/
		VideoOutput(const VideoOutput &vout);

		/** @brief Destructs a VideoOutput.
		  * Description
		  * A VideoOutput object will be destruct if no more references
		  * exist. All decoders will automatically disconnect and the
		  * screen goes to black by stays in VideoMode.                
		*/
		virtual ~VideoOutput();

		/** @brief Activate a video mode.
		  * Description
		  * Select a video mode and color space for a VideoOutput. Some
		  * video mode may support different color spaces, that makes it
		  * important to select the color space you want to output. For
		  * \example dual HD-SDI may support YCbCr and RGB. It's an
		  * important information to give the firmware the chance to
		  * convert the input video to the correct output color space.<p />
		  * \Note: The current firmware version does not support a color
		  * conversion. The color space selection will only effect the
		  * color for inactive areas inside a frame or if no video is
		  * displayed.
		  * @param mode   VideoMode
		  * @param color  ColorSpace or default for the default color
		  *               space of the video mode.
		  * @return Returns MMRC_Ok on successful activation of the video
		  * mode.
		  * @retval MMRC_Ok                          successful operation
		  * @retval MMRC_MVC2_OutputNotFound         the video output
		  *                                          could not be location
		  *                                          on the card
		  * @retval MMRC_MVC2_VideoModeNotSupported  the video mode is not
		  *                                          supported             
		*/
		TMmRc setVideoMode(const VideoMode mode, const ColorSpace color = ColorSpace(ColorSpace::Space_Default));
		
		/** @brief Get the active video mode.
		  * Description
		  * This method returns the currently active video mode.
		  * @return Returns MMRC_Ok on successful retrieval of the video
		  * mode.
		  * @retval MMRC_Ok  successful operation                       
		*/
		VideoMode getActiveVideoMode();

		/** @brief Enables audio embedding.
		  * Description
		  * This method controls if embedded audio into HD-SDI is used or
		  * not. All 16 channels will be embedded into channel A.
		  * @param enable  true to enable embedding else false
		  * @return MMRC_Ok if the audio embedding was successfully
		  * enabled.
		  * @retval MMRC_Ok                  success
		  * @retval MMRC_MVC2_InvalidObject  if the VideoOutput object is
		  *                                  empty                       
		*/
		TMmRc setAudioEmbedding(bool enable);

		/** @brief Sets the position of the video.
		  * Description
		  * A video can be freely positioned on screen. This is
		  * especially useful if the input video is smaller or bigger
		  * than the output video mode requires it. See <link mvc2::FramePosition, FramePosition>
		  * for greater detail on the exact position. The default
		  * position is centered in both directions.
		  * @param pos                 destination position of the video
		  * @param transistionSpeed    VideoOutput\:\:EFFECTSPEED how fast
		  *                            the transition from current to
		  *                            destination position is completed
		  * @param transisitionEffect  VideoOutput\:\:MOVEEFFECT selects
		  *                            the movement effect for the
		  *                            transition
		  * @return MMRC_Ok on success, otherwise an error code.
		  * @retval MMRC_Ok                  success
		  * @retval MMRC_MVC2_InvalidObject  if the VideoOutput object is
		  *                                  empty                                               
		*/
		TMmRc setPosition(const FramePosition &pos, EFFECTSPEED transistionSpeed = Speed_Immediately, MOVEEFFECT transisitionEffect = MoveEffect_Linear);

		/** @brief Select the behavior of a VideoOutput.
		  * Description
		  * This method selects the video which should be output on the
		  * primary channel. It's used in conjunction with two linked
		  * VideoOutputs if the secondary output is only virtual. It is
		  * possible to select the video from decoder at VideoOutput one
		  * \or two, or blend the video from both outputs together. The
		  * main functionality is to dynamically switch between two
		  * channels of a 3D video by having only one projector/monitor
		  * connected.<p />
		  * See <link mvc2::VideoOutput::OUTPUTACTIVITY, VideoOutput::OUTPUTACTIVITY>
		  * for possible settings.
		  * @param activity        One of the VideoOutput\:\:OUTPUTACTIVITY
		  *                        values 
		  * @param secondaryAlpha  For
		  *                        Activity_Primary_Secondary_as_Overlay
		  *                        or
		  *                        Activity_Secondary_Primary_as_Overlay a
		  *                        alpha value between 1.0 and 0.0 can be
		  *                        given to use for the overlay 
		  * @return MMRC_Ok if the setup was successfully completed,
		  * \otherwise an error code.
		  * @retval MMRC_Ok                  success 
		  * @retval MMRC_MVC2_InvalidObject  if the VideoOutput object is
		  *                                  empty                                   
		*/
		TMmRc setOutputActivity(OUTPUTACTIVITY activity, float secondaryAlpha = 1.0f);

		TMmRc enableDeGhosting(const int32_t *parameter = 0, uint32_t size = 0, uint32_t type = 0);

		TMmRc disableDeGhosting();

		/** @brief Assignment operator.
		  * Description
		  * This operator copies the reference of a VideoOutput to a new
		  * \object.
		  * @param vout  source VideoOutput object
		  * @return Reference to the destination VideoOutput object.<p />
		*/
		VideoOutput &operator =(const VideoOutput &vout);

		/** @brief Bool operator.
		  * Description
		  * Check if a VideoOutput object is empty or not. Returns false
		  * for an empty object.                                        
		*/
		operator bool() const;

	protected:
      /* @brief Internal use only.
      Description
      The method returns the private object. It's for internal use
      \only.
      @return Private object.
      */
      VideoOutputPrivate *getPrivate() const;

		VideoOutputPrivate *m_outputPrivate;
	};

	class MVC2_API AudioOutputStatus
	{
	public:
		enum TIME_RESOLUTION
		{
			Resolution_100ms			= 100,
			Resolution_1s				= 1000,
		};
	public:
		AudioOutputStatus(const MvcDevice &mvcdev, uint32_t index, TIME_RESOLUTION res = Resolution_1s);
		AudioOutputStatus(AudioOutputStatusPrivate *priv);
		AudioOutputStatus(const AudioOutputStatus &other);
		AudioOutputStatus();

		virtual ~AudioOutputStatus();

		uint32_t getSampleLevel(uint32_t channel) const;
		float getSampleLevelDB(uint32_t channel) const;
		uint32_t getSampleActivity(uint32_t channel) const;
		uint32_t getOvershootSamples(uint32_t channel) const;
		uint32_t getOvershootLength(uint32_t channel) const;
		bool isChannelActive(uint32_t channel) const;

		/* @brief Returns if the object is valid.
		Description
		\Returns true if the object is valid or false if the object
		is invalid.                                                 */
		operator bool() const;

		AudioOutputStatus &operator =(const AudioOutputStatus &other);

	protected:
		AudioOutputStatusPrivate *m_StatusPriv;
	};


	class MVC2_API AudioOutput
	{
      friend class PCMDecoder;
		friend class SecurityAccess;

	public:
		/* Possible properties of a audio output. */
		enum AUDIOPROPERTY
		{
			AudioProperty_Default  					      = (0),    /* default value */
			
			AudioProperty_16bit_Samples					= (1<<0), /* \Output supports 16 bit samples. */
			
			AudioProperty_20bit_Samples					= (1<<1), /* \Output supports 20 bit samples. */
			
			AudioProperty_24bit_Samples					= (1<<2), /* \Output supports 24 bit samples. */
			
			AudioProperty_Bitstream_Support				= (1<<3), /* \Output can be used to transfer a compressed audio bit
			   stream.                                                */
			
			AudioProperty_Video_Embedding					= (1<<4)  /* \Output will transfer the data over video using audio
			   embedding.                                            */
		};

	public:
		AudioOutput();
		AudioOutput(const AudioOutput &other);

      /* @brief Create AudioOutput object.
      Description
      This method creates a object for a physical or virtual audio
      \output. Before the object will be created the list of
      available outputs will be parsed to find an output which fits
      best to the AudioProperty.
      @param resultPointer  Pointer where to store the result in. if 
                            0 no result will be written. 
      @param mvcDevice      MvcDevice to create the output on
      @param audioProperty  optional properties to influence the
      \output selection
      @return MMRC_Ok if the output could be created, else an
      error.
      @retval MMRC_Ok  \Output successfully created                 */
		AudioOutput(TMmRc * resultPointer, const MvcDevice &mvcDevice, uint32_t numberOfChannels, uint32_t audioProperty = 0);

      virtual ~AudioOutput();

		/** @brief Sets audio delay.
		  * Description
		  * This method sets the delay between the audio output and the
		  * clock playback control, respectively the audio video delay.
		  * The delay is processed on the fly, so any change will be
		  * audioable immediately.
		  * @param delay  delay in milli-seconds, any positive or negative
		  *               value is allowed, too big values may influence
		  *               buffer fullness or overall playback
		  * @return MMRC_Ok if new value was successfully setup.
		  * @retval MMRC_Ok  \Return value 1                              
		*/
		TMmRc setOutputDelay(int32_t delay);

		/** @brief Gets the current audio output delay
		  * Description
		  * The current active audio output delay can be read by this
		  * function.
		  * @return Returns the audio delay in milli-seconds.<p />   
		*/
		int32_t getOutputDelay() const;

		/** @brief Selects audio channels to mute.
		  * Description
		  * This method mutes selected audio channels. The mask is a bit
		  * array with one bit per channel starting with the lowest bit
		  * (bit 0) for the first channel. A 1 in the mask means the
		  * channel is muted.
		  * @param mask  channel mask with bits set for muting
		  * @return MMRC_Ok if muting setup was successful.<p />
		  * \Return Value List
		  * @retval MMRC_Ok  on success                                 
		*/
		TMmRc setChannelMute(uint32_t mask);

		/** @brief Gets channel muting
		  * Description
		  * This functions gets the current channel muting mask from the
		  * \output. The mask has a bit for every channel of the output,
		  * starting from bit 0 for channel 0. A 1 in the mask means the
		  * channel is muted.
		  * @return Returns output muting mask.<p />                    
		*/
		uint32_t getChannelMute() const;

		/** @brief Sets the audio output frequency.
		  * Description
		  * The audio output can run at 48kHz or at 96kHz. This method is
		  * used to set the frequency of the output.
		  * @param frequency  \sample frequency in Hz, values of 48000 or
		  *                   96000 are allowed
		  * @return MMRC_Ok on success, otherwise an error code.
		  * @retval MMRC_Ok                                Sucessfull
		  *                                                frequency setup
		  * @retval MMRC_MVC2_SampleFrequencyNotSupported  unsupported
		  *                                                \sample
		  *                                                frequency      
		*/
		TMmRc setOutputFrequency(uint32_t frequency);

		/** @brief Gets the current output frequency
		  * Description
		  * This function reads the currently active output frequency
		  * from the audio output.
		  * @return Returns the sample frequency in Hz.<p />         
		*/
		uint32_t getOuputFrequency() const;

		/** @brief Sets audio user data.
		  * Description
		  * A AES/3 audio channel can transfer optional channel with user
		  * bits. It is an array of 192 bits which can totally user
		  * defined or comply to AES/3 or IEC 60958 standard. This method
		  * can be used to set these 192 bits per audio channel. Changes
		  * take place immediately.
		  * @param channel   channel number of that audio output
		  * @param userData  an array of up to 24 bytes with the userdata,
		  *                  lsb first
		  * @param size      size of the userData array, if greater than
		  *                  24, the array will be truncated to 24, if
		  *                  lower than 24, the array will be filled up
		  *                  with zeros
		  * @return MMRC_Ok in successful setup.
		  * @retval MMRC_Ok                       on success
		  * @retval MMRC_MM_API_Index2OutOfRange  if the channel number is
		  *                                       not available inside
		  *                                       this output             
		*/
		TMmRc setUserData(uint32_t channel, const uint8_t *userData, uint32_t size);

		TMmRc setVolume(uint32_t firstChannel, uint32_t numChannels, const float *volumeArray);
		TMmRc setVolumeDB(uint32_t firstChannel, uint32_t numChannels, const float *volumeArray);

		TMmRc getAudioOutputStatus(AudioOutputStatus &status, AudioOutputStatus::TIME_RESOLUTION res = AudioOutputStatus::Resolution_1s) const;

		uint32_t getNumberOfChannels() const;

		AudioOutput &operator =(const AudioOutput &other);

		/* @brief Returns if the object is valid. 
                                             */
		operator bool() const;

	private:
      /* @brief Internal method.
      Description
      The method returns the private object.
      @return Private object.
      */
      AudioOutputPrivate *getPrivate() const;

      AudioOutputPrivate *m_outputPrivate;
	};

	class MVC2_API ExternalConnector
	{
	public:
		ExternalConnector();
		ExternalConnector(const ExternalConnector &other);
		ExternalConnector(TMmRc * resultPointer, const MvcDevice &mvcDevice);

		virtual ~ExternalConnector();

		ExternalConnector &operator =(const ExternalConnector &other);

		/* @brief Returns if the object is valid. 
		*/
		operator bool() const;

		TMmRc readI2C(uint16_t deviceAddr, uint8_t registerAddr, uint8_t * buffer, uint32_t bufferSize);
		TMmRc readI2C(uint16_t deviceAddr, uint8_t * buffer, uint32_t bufferSize);
		TMmRc writeI2C(uint16_t deviceAddr, const uint8_t * buffer, uint32_t bufferSize);

	protected:
		ExternalConnector(ExternalConnectorPrivate *dev);

		ExternalConnectorPrivate *m_ConnectorPrivate;

	};

	class MVC2_API VSCConnector : public ExternalConnector
	{
	public:
		enum DIRECTION
		{
			Dir_Input			= 0,
			Dir_Output			= 1,
		};

		enum OUTPUTMODE
		{
			Out_OpenDrain		= 0,
			Out_Driver			= 1,
		};

		enum TRIGGERMODE
		{
			Trigger_ActiveLow		= 0,
			Trigger_ActiveHigh	= 1,
			Trigger_FallingEdge	= 2,
			Trigger_RisingEdge	= 3,
		};

	public:
		VSCConnector();
		VSCConnector(const VSCConnector &other);
		VSCConnector(TMmRc * resultPointer, const MvcDevice &mvcDevice);

		virtual ~VSCConnector();

		// preliminary functions:
		TMmRc setGPIOConfig(uint32_t dir, uint32_t dir_mask, uint32_t output_mode);
		TMmRc setGPIOValue(uint32_t val, uint32_t value_mask);
		TMmRc getGPIOValue(uint32_t *val);

		TMmRc waitForGPIOInterrupt(uint32_t *result_gpios, uint32_t gpios, uint32_t trigger_mode);
		TMmRc abortGPIOInterrupt();

	protected:
		VSCConnector(ExternalConnectorPrivate *dev);
	};

}

#endif /* _MVC2API_OUTPUT_H_ */
