/** mvc2api
  * * ----------------------------------------------------------- *
  * 
  * * ----------------------------------------------------------- *
  * Copyright (C) 2006-2012 MikroM GmbH - All Rights Reserved      
*/
#ifndef _MVC2API_H_
#define _MVC2API_H_

#include "mvc2api_types.h"
#include "mvc2api_decoder.h"
#include "mvc2api_output.h"
#include "mvc2api_device.h"
#include "mvc2api_overlay.h"

#include <new>
#include <ctime>


/** Namespace of all MVC20x API classes.
*/
namespace mvc2
{
	class LogAccessPrivate;
	class PlaybackControlPrivate;
	class ProjectorAccessPrivate;

	
   class MVC2_API PlaybackControl
	{
	public:
		/* Playback state. */
		enum PLAYBACK_STATE
		{
			Stopped = 0, /* \ \ 
			   Description
			   The card is in stopped state. No video will be displayed
			   until <link mvc2::PlaybackControl::run, run()> or <link mvc2::PlaybackControl::pause, pause()>
			   was called.                                                                                    */
			
			Running = 1, /* \ \ 
			   Description
			   The card is currently in running state. Video and audio will
			   be played.                                                   */
			
			Paused = 2,/* \ \ 
			   Description
			   The MVC card is in paused state. A freeze frame will be
			   displayed or the card waits for the first decoded frame. */
			
		};

	public:
      PlaybackControl();

      /* @brief Creates a PlaybackControl object.
      Description
      This method will create a PlaybackControl object and attach
      it to a given <link mvc2::MvcDevice, MvcDevice>.
      @param resultPointer  Pointer where to store the result in. if 
      0 no result will be written. 
      @param mvcdev    MvcDevice which should be controlled
      @return MMRC_Ok will be returned if the creation was
      successful, otherwise an error.
      @retval MMRC_Ok          operation successfully completed
      @retval MMRC_DeviceBusy  object could not be created because
      the device is already in use        */
      PlaybackControl(TMmRc * resultPointer, const MvcDevice &mvcdev);

		/* @brief Default destructor
		   Description
		   The PlaybackControl object will be freed and all of it's
		   resources.                                               */
		virtual ~PlaybackControl();

		/* @brief Starts the playback.
		   Description
		   After calling the run() method, the playback will start.
		   @return MMRC_Ok will be returned if the operation was
		   successful, otherwise an error.
		   @retval MMRC_Ok  operation successfully completed        */
		TMmRc run();
		/* @brief Pauses the video/audio
		   Description
		   After calling pause() the video and audio will freeze
		   immediately. Audio mutes and video displays a still frame. If
		   the video is not running, pause can be activated before. This
		   display the first decoded video frame right after it was
		   decoded. The playback can then be started with <link mvc2::PlaybackControl::run, run()>
		   method.
		   @return MMRC_Ok will be returned if the operation was
		   successful, otherwise an error.
		   @retval MMRC_Ok  operation successfully completed                                       */
		TMmRc pause();
		/* @brief Stops the playback.
		   Description
		   The stop() method will stop the whole playback and display a
		   stop specific image (normally this is a black frame). All
		   decoder buffers will be flushed.
		   @return MMRC_Ok will be returned if the operation was
		   successful, otherwise an error.
		   @retval MMRC_Ok  operation successfully completed            */
		TMmRc stop();

		/** @brief Single step a frame.
		  * Description
		  * This function causes the playback to advance by a number of
		  * frames. If the playback is not already paused, pause will be
		  * activated. Normal usage would be singeStep(1), to step to the
		  * next frame. A singleStep(10) would skip 9 frames and show the
		  * following frame.
		  * @param NumberOfFrames  number of frames to advance the playback
		  * @return MMRC_Ok on success, otherwise an error.
		  * @retval MMRC_Ok  operation successfully completed              
		*/
		TMmRc singleStep(int32_t NumberOfFrames);

		/** @brief Start playback with specific speed.
		  * Description
		  * This function works like run() function, but here a running
		  * speed can be set. The speed should be bigger than 0 and not
		  * to high (not more than 3), because maximum the speed is
		  * limited by the decoding speed. For higher playback speed the
		  * player need to skip frames before transferring them to the
		  * card.
		  * @param speed  playback speed, 1.0 is normal speed
		  * @return Returns MMRC_Ok if playback started successfully,
		  * \otherwise an error.
		  * @retval MMRC_Ok  playback started                           
		*/
		TMmRc runSpeed(float speed);

		/* @brief Gets the current state.
		   Description
		   This method will return the current active PlaybackControl
		   state.
			@param timeStamp  current time stamp used for play out (in 90kHz ticks)
		   @return A <link mvc2::PlaybackControl::PLAYBACK_STATE, PlaybackControl::PLAYBACK_STATE>
		   will be returned.
		                                                                                           */
		PLAYBACK_STATE getState(uint32_t *timeStamp = 0);

		/* @brief Connect the playback control with a decoder.
		   Description
		   To control decoders with a playback control, the playback
		   control must be connected to the decoders.
		   @param dec  decoder which will be connected to the playback
		               control
		   @return MMRC_Ok if the connect succeeded, else an error
		   @retval MMRC_Ok  connection successful                      */
		TMmRc connect(const MvcDecoder& dec);

		/* @brief Disconnect a decoder from the playback control.
		   @param dec  decoder which will be disconnected from the playback
			control
		   @return MMRC_Ok if the disconnect succeeded, else an error
		   @retval MMRC_Ok  disconnect successful                      */
		TMmRc disconnect(const MvcDecoder& dec);

		/** @brief Waits for end of stream.
		  * Description
		  * This function will return if the end of stream marker is
		  * received by all decoders connected to the playback control.
		  * The end of stream maker will be queue using <link mvc2::MvcDecoder::setEndOfStream, setEndOfStream()>
		  * at a decoder. The function can be aborted by calling <link mvc2::PlaybackControl::stop, stop()>,
		  * which resets all decoders and there end of stream marker.
		  * @return MMRC_Ok if end of stream was reached.
		  * @retval MMRC_Ok       end of stream reached
		  * @retval MMRC_TimeOut  waitForEndOfStream was aborted, probably
		  *                       by calling stop().                                                             
		*/
		TMmRc waitForEndOfStream();
		
		TMmRc flush(uint32_t keepTime, uint32_t *nextTimeStamp);

		TMmRc setSyncSlave(bool enable);
		
      PlaybackControl(const PlaybackControl& other);
      PlaybackControl& operator= (const PlaybackControl& other);

      operator bool() const;

	private:
		/* @brief Internal constructor (API private)
		   Description
		   Please use create() method to create PlaybackControl objects. */
      explicit PlaybackControl(PlaybackControlPrivate *playbackPrivate);

      PlaybackControlPrivate *m_PlaybackPrivate;

	};


	/* Log Message class */
	class MVC2_API LogMessage
	{
	public:
		/* Severity level of the LogMessage. */
		enum Severity
		{
			SeverityNone			= 0, /* LogMessage is empty and therefore it doesn't have a severity level. */
			SeverityTrace			= (1<<0), /* LogMessage is a trace message. */
			SeverityInfo			= (1<<1), /* LogMessage is an information. */
			SeverityWarning		= (1<<2), /* LogMessage is a warning. */
			SeverityError			= (1<<3)  /* LogMessage is an error. */
		};

		/* Identifier for the source of the LogMessage. */
		enum LogId
		{
			IdNone					= 0,			/* LogMessage is empty and therefore it doesn't have an identifier. */
			IdApi						= (1<<0),	/* Message create by MVC API. */
			IdDriver					= (1<<1),	/* Message create by MVC driver. */
			IdSecurityManager		= (1<<2),	/* Message create by the security manager on the MVC card. */
			IdFirmware				= (1<<3)		/* Message create by the MVC card's firmware. */
		};

		/* The sub id identifies a specific source of the LogMessage for messages with LogId set to IdFirmware.
			The bits are used as a mask, e.g. messages related to a video output will have bit0 and bit3 set. */
		enum LogSubIdFirmware
		{
			SubIdVideo				= (1<<0),	/* LogMessage is related to video. */
			SubIdAudio				= (1<<1),	/* LogMessage is related to audio. */
			SubIdDecoding			= (1<<2),	/* LogMessage is related to decoding processes. */
			SubIdOutput				= (1<<3),	/* LogMessage is related to outputs. */
			SubIdControl			= (1<<4),	/* LogMessage is related to controlling. */
			SubIdSecurity			= (1<<5),	/* LogMessage is related to security. */
			SubIdNetwork			= (1<<6)		/* LogMessage is related to networking. */
		};

	public:
		/* @brief default constructor
		   Description
		   The constructor create a empty LogMessage object.
			If (preallocatedSize > 0) a message buffer gets allocated already. 
			This can be used to avoid multiple free/allocates during message manipulation.*/
		LogMessage(uint32_t preallocatedSize = 0);
		/* @brief Default destructor.
		   Description
		   The destructor frees all resources of the LogMessage. */
		~LogMessage();

		/* @brief specific constructor
		   Description
		   This constructor creates a complete LogMessage object.
		   @param sev        Severity level
		   @param id         Id
		   @param subid      SubId
		   @param counter    Message counter
		   @param timestamp  Time stamp in milliseconds
		   @param msgstr     pointer to the message string (zero
		                     terminated)
		   @param msgsize    number of bytes of the message string */
		LogMessage(const Severity sev, const LogId id, const uint32_t subid, const uint32_t counter, const uint64_t timestamp, const char *msgstr, const uint32_t msgsize);

		/* @brief LogMessage copy constructor.
		   Description
		   This constructor creates a new LogMessage as exact copy of
		   the source LogMessage.
		   @param msg  source LogMessage                              */
		LogMessage( const LogMessage& other );

		/* @brief Assignment operator
		   Description
		   This operator copies all information from one LogMessage
		   \object to another.
		   @param msg  \input LogMessage object
		   @return True if the LogMessage is not empty after the copy.
		                                                               */
		LogMessage &operator =(const LogMessage &other);

		operator bool() const;

		/* @brief Return the severity level.
		   Description
		   Gets the severity level of the log message.
		   @return Severity level of <link mvc2::LogMessage::Severity::SeverityNone, SeverityNone>
		   for an empty log message.                                                               */
		Severity getSeverity() const;

		/* @brief Return the message id.
		   Description
		   \Returns the Id of the log message.
		   @return A <link mvc2::LogMessage::LogId, LogId> or <link mvc2::LogMessage::LogId::IdNone, IdNone>
		   if the message is empty.                                                                          */
		LogId getId() const;

		/* @brief Return sub id.
		   Description
		   Gets the sub id of the log message.
		   @return Sub id of the message.      */
		uint32_t getSubId() const;

		/* @brief Return the message counter.
		   Description
		   Gets the message counter of the current log message.
		   @return The message number or zero for a empty log message.
		                                                               */
		uint32_t getMessageCounter() const;

		/* @brief Return message string.
		   Description
		   Gets the pointer to the internal string buffer with the
		   message string. This string is valid as long as the object
		   exists and it's not assigned from another LogMessage object.
		   @return Pointer to a string or NULL for a empty log message.
		                                                                */
		char *getMessageString() const;

		/* @brief Return valid message bytes.
		   Description
		   Gets the valid message bytes contained at address obtained by
		   <link mvc2::LogMessage::getMessageString, getMessageString()>.
		   Termination bytes are not included.
		   @return Valid message bytes, or zero for a empty log message.  */
		uint32_t getMessageStringLength() const;

		/* @brief Modify the message.
		   Description
		   Sets the message information. The message string will be
		   copied to the LogMessage object.
		   @param sev          severity level
		   @param id           id
		   @param subid        sub id
		   @param counter      message counter
		   @param timestamp    time stamp
		   @param message      pointer to the message string
		   @param messageSize  size in bytes of the message
		                       string                               */
		void setMessage(const Severity sev, const LogId id, const uint32_t subid, const uint32_t counter, const uint64_t timestamp, const char * message, uint32_t messageSize);
			
		/* @brief Return the time stamp as ANSI-C time.
		   Description
		   This method returns the time of the message as time_t type to
		   be used with ANSI-C time functions. The time is a counter of
		   seconds starting from 1.1.1970.
		   @return Number of seconds starting from 1.1.1970 of the Log
		   message.
		*/
		time_t getCTime() const;

		/* @brief Returns microseconds of the message.
		   Description
		   Get the number of microseconds of the log message. This is
		   useful in conjunction with <link mvc2::LogMessage::getCTime@const, getCTime()>.
		   @return Number of microseconds of the message.
		*/
		uint32_t getMicroSeconds() const;
		
		/* @brief Return timestamp.
		   Description
		   Gets the time stamp of the message. The time is counted in
		   microseconds starting from 1.1.1970. Time zone is UTC.
		   @return Time of the message or zero for an empty log message.
		*/
		uint64_t getTimeStamp() const;

	private:
		friend class LogAccess;

		Severity m_severity;
		LogId m_logId;
		uint32_t m_subId;
		uint32_t m_messageCounter;
		uint64_t m_timeStamp;
		uint32_t m_messageSize;
		char *m_message;
	};

	/* This class is used to access the debug log of the MVC card. */
	class MVC2_API LogAccess
	{
	public:
		enum TIMEOUT
		{
			WaitInfinite = 0xffffffff, /* Wait forever. */
			DonotWait = 0 /* Do not wait. Return function immediately. */
		};

	public:
      LogAccess();

      /* @brief Create LogAccess object.
         Description
         This method creates a LogAccess object.
         @param resultPointer  Pointer where to store the result in. if 
         0 no result will be written. 
         @param mvcdev                  MvcDevice object to create a
         LogAccess object to
         @param ignoreExistingMessages  ignore all existing messages
         the log buffer and output only
         new ones
         @return MMRC_Ok for success, else an error
         @retval MMRC_Ok               object creation successful
         @retval MMRC_MVC2_DeviceBusy  MvcDevice is busy and cannot be
         accessed                        */
      LogAccess(TMmRc * resultPointer, const MvcDevice &mvcdev, bool ignoreExistingMessages);

      LogAccess(const LogAccess& other);
      LogAccess& operator=(const LogAccess& other);

      /* @brief Returns if the object is valid.
         Description
         \Returns true if the object is valid or false if the object
         is invalid.                                                 */
      operator bool() const;

		virtual ~LogAccess();

		/* @brief Set the message filter.
		   Description
		   The log messages can be filtered before they will be
		   returned, which makes the log access much more efficient. By
		   default all message will be return without any filter.
		   @param severityMask  wanted severity levels
		   @param idMask        wanted id's
		   @param subIdValue    wanted sub id's
		   @param subIdMask     mask for the wanted sub id's
		   
		                                                                */
		void setMessageFilter(uint32_t severityMask, uint32_t idMask, uint32_t subIdValue, uint32_t subIdMask);
		
		/* @brief Get a log message.
		   Description
		   This method will return the currently available log message
		   \or wait for a new message to come in.
		   @param msg      LogMessage object which receives the message
			@param moreMessagesAvailable  true, if there are messages waiting, else false
		   @param timeout  timeout in milliseconds to wait for a new
		                   message
		   @return MMRC_Ok or an error.
		   @retval MMRC_Ok            log message received
		   @retval MMRC_Timeout  timeout occurred                  */
		TMmRc getMessage(LogMessage &msg, bool * moreMessagesAvailable, uint32_t timeout = WaitInfinite);

		/* @brief Aborts a pending <link mvc2::LogAccess::getMessage@LogMessage &amp;@bool *@uint32_t, getMessage()>
		   \operation.
		   Description
		   The <link mvc2::LogAccess::getMessage@LogMessage &amp;@bool *@uint32_t, getMessage()>
		   method will wait for an incoming message. There may
		   situations where this waiting should be aborted (e.g. to
		   close a application). The <link mvc2::LogAccess::getMessage@LogMessage &amp;@bool *@uint32_t, getMessage()>
		   method will quickly return with a new message or a timeout.                                                 */
		void abortGetMessage();

	private:
		explicit LogAccess(LogAccessPrivate *priv);

	private:
		LogAccessPrivate *m_LogAccessPrivate;
	};

	/** @brief Interface to the projector connected to the MVC201.
	  * Description
	  * This class is used to communicate with the projector
	  * connected to the MVC201. Please check TI or OEM documentation
	  * for available commands.                                      
	*/
	class MVC2_API ProjectorAccess
	{
	public:
      ProjectorAccess();
      /** @brief Constructs a projector access object.
        * Description
        * A projector communication tunnel will be created and the
        * connection to the projector will be established.
        * @param resultPointer  return code from the creation
        * @param mvcdev         base mvc device
        * @param ipaddr         optional ip address, default ICP ip
        *                       address
        * @param port           optional network port, default 0xaaa1
      */
      ProjectorAccess(TMmRc * resultPointer, const MvcDevice &mvcdev, const char *ipaddr = 0, uint16_t port = 0xaaa1);

      ProjectorAccess(const ProjectorAccess& other);
      ProjectorAccess& operator=(const ProjectorAccess& other);
		virtual ~ProjectorAccess();

      /* @brief Returns if the object is valid.
         Description
         \Returns true if the object is valid or false if the object
         is invalid.                                                 */
      operator bool() const;

		/** @brief Send and receive a projector command.
		  * Description
		  * Send a projector command and receive the response using the TI protocol. The data
		  * starts with 3 bytes command followed by 2 bytes size and data
		  * block. Check sum is calculated and checked automatically.
		  * Please make sure to initialize the sizeOut variable with the
		  * space of the data array before calling this function.
		  * @param data     \input data block and output buffer
		  * @param sizeIn   number of bytes to send
		  * @param sizeOut  size of data block and number of bytes
		  *                 received
		  * @return If MMRC_Ok is returned the sizeOut variable is set
		  * with the number of bytes received.<p />                      
		*/
		TMmRc commandIo(uint8_t *data, uint32_t sizeIn, uint32_t *sizeOut);

		TMmRc rawSend(uint8_t *data, uint32_t sizeIn);
		TMmRc rawRecv(uint8_t *data, uint32_t *sizeOut);

		/** @brief Login into the projector.
		  * Description
		  * This function is used to authenticate the projector
		  * connection. See TI and OEM documentation for available
		  * credentials.
		  * @param username  pointer to the user name
		  * @param password  pointer to the password
		  * @return Return MMRC_Ok if login was successful, otherwise an
		  * error.<p />                                                 
		*/
		TMmRc login(const char *username, const char *password);
		/** @brief Logout from projector
		  * Description
		  * Remove the authentication from the projector connection.
		  * @return MMRC_Ok if logout was successful.<p />          
		*/
		TMmRc logout();

	private:
		ProjectorAccessPrivate *m_ProjectorAccessPrivate;
	};

}

#endif /* _MVC2API_H_ */
