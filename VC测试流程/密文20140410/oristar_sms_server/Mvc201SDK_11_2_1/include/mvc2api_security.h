#ifndef _MVC2API_SECURITY_H_
#define _MVC2API_SECURITY_H_

#include "mvc2api.h"


namespace mvc2
{
	class SecurityAccessPrivate;
	class MVC2_API SecurityAccess;

	class MVC2_API TamperStatus
	{
	public:
		enum TAMPER_SWITCHES
		{
			SWITCH_TAMPER_1			= (1<<0),
			SWITCH_TAMPER_2			= (1<<1),
			SWITCH_MARRIAGE_1			= (1<<2),
			SWITCH_MARRIAGE_2			= (1<<3),
			SWITCH_DOOR_1				= (1<<4),
			SWITCH_DOOR_2				= (1<<5)
		};

		enum TAMPER_EVENTS
		{
			EVENT_DOOR_OPEN_1			= 0,
			EVENT_DOOR_OPEN_2 		= 1,		
			EVENT_DOOR_CLOSE_1		= 2,		
			EVENT_DOOR_CLOSE_2		= 3,
			EVENT_DIVORCE_1			= 4,
			EVENT_DIVORCE_2			= 5,
			EVENT_TAMPER_1 			= 6,
			EVENT_TAMPER_2 			= 7,
			EVENT_POWER_LOSS			= 8, 
			EVENT_POWER_OV				= 9,
			EVENT_BAT_REMOVE			= 10,
			EVENT_BAT_NEW				= 11
		};

	public:
		TamperStatus();

		uint32_t getSwitchStatus() const;

		bool getTamperEvent(uint32_t index, uint32_t *tamperEvent, time_t *tamperTimer) const;

	private:
		uint32_t m_switches;
		uint32_t m_events[32*2];

		friend class SecurityAccess;
	};

	class MVC2_API SecurityAccess
	{
	public:
		/** Media type for decryption
		*/
		typedef enum _MediaType
		{
			MEDIA_TYPE_Video = 0, /** Media type is video
			*/
			
			MEDIA_TYPE_Audio = 1, /** Media type is audio
			*/
			
			MEDIA_TYPE_Other = 2 /** Media type is not audio nor video
			*/
			
		} MediaType;

		typedef enum _SymmetricCryptoOperation
		{
			AES128OP_cbc_enc = 0,
			AES128OP_cbc_dec = 1,
			AES128OP_ecb_enc = 2,
			AES128OP_ecb_dec = 3,
			TrippleDes_cbc_enc = 4,
			TrippleDes_cbc_dec = 5,
			MMTrippleDes_cbc_enc = 8,
			MMTrippleDes_cbc_dec = 9,
			AES128OP_cbc_dec_fpga = 10,
			AES256OP_cbc_enc = 11,
			AES256OP_cbc_dec = 12,
		} SymmetricCryptoOperation;

		typedef enum _HashAlgorithm
		{
			Sha1=0,
			Sha256=1,
			Md5=2,
			Sha1WithZlib=3
		} HashAlgorithm;

		typedef enum _SoftwareId
		{
			SW_ID_SecApplication = 0,
			SW_ID_MainFirmware = 1,
			SW_ID_SecBootloader = 2,
			SW_ID_MainBootloader = 3,
			SW_ID_MainFPGA = 4
		} SoftwareId;

		static const size_t DateLen=12;
		static const size_t TimeLen=9;
		static const size_t CineLink2KeyLen = 16;
		static const size_t CineLink2AttrLen = 8;

		typedef struct _CineLink2Para
		{
			uint8_t m_key[SecurityAccess::CineLink2KeyLen];
			uint8_t m_attribute[SecurityAccess::CineLink2AttrLen];
			uint16_t m_keyID;
			uint16_t m_nextKeyID;
		} CineLink2Para;


	public:
		/** @brief Default destructor.
		  * Description
		  * The destructor will free all resources allocated by this
		  * \object. Any setups will be kept. Local Link Encryption and
		  * key storage remain untouched.                              
		*/
		virtual ~SecurityAccess();

		/** @brief Default constructor.
		  * Description
		  * This constructor will create a empty, non-functional
		  * SecurityAccess object.                              
		*/
		SecurityAccess();
		/** @brief Creates a SecurityAccess object.
		  * Description
		  * The SecurityAccess is an interface for a security manager
		  * which is implemented on the host system (e.g. a Player
		  * software). All function of the SecurityAccess are normally
		  * controlled by the on board security manager. If it is not
		  * available or the customer don't want to use it, the main
		  * security features can be accessed by this class.
		  * @param resultPointer  Return code from the creation
		  * @param mvcDevice      MvcDevice object for which the object
		  *                       should be created
		  * 
		  * @remarks Using this class for security features is not save.
		  * Function calls can be easily traced by third party programs.
		  * It should only be used for debugging or if there's no
		  * security manager on board. The only safe and DCI complaint way
		  * is to use the on board security manager.<p />
		  * If the on board security manager is active, none of these
		  * functions will have an effect!                               
		*/
		SecurityAccess(TMmRc * resultPointer, const MvcDevice &mvcDevice);

		/** @brief Default copy constructor.
		  * Description
		  * A new SecurityAccess will be created from input
		  * SecurityAccess.
		  * @param other  \Input SecurityAccess            
		*/
		SecurityAccess(const SecurityAccess& other);
		/** @brief Default assignment operator.
		  * Description
		  * Copies on all information from one SecurityAccess object.
		  * @param other  source object
		  * @return Pointer to itself.<p />                          
		*/
		SecurityAccess& operator=(const SecurityAccess& other);

		/** @brief Disable local link encryption.
		  * Description
		  * The HD-SDI connection between the MVC card and the projector
		  * is called local link. The method will deactivate any
		  * encryption for the HD-SDI connection of a video output. This
		  * will deactivate CineLink(TM) 1 and CineLink(TM) 2.
		  * @param output  VideoOutput to disable the output encryption
		  * @param link    link number inside the VideoOutput
		  * @return Returns MMRC_Ok on successful deactivation.
		  * @retval MMRC_Ok                   successful operation
		  * @retval MMRC_MVC2_OutputNotFound  the output could not be
		  *                                   located on the card       
		*/
		TMmRc disableLLE(const VideoOutput &output, uint32_t link);

		/** @brief Enable CineLink(TM) for a video output.
		  * Description
		  * This method enable the CineLine(TM) 1 for a given video
		  * \output. CineLink(TM) is a encryption algorithm for the
		  * HD-SDI transmission between the MVC card and the projector.
		  * Due to the support for multi link HD-SDI of the MVC card
		  * (e.g. dual HD-SDI). You have to specify the link where to
		  * enable the encryption.
		  * @param output  VideoOutput object 
		  * @param link    link number of the video output
		  * @param key     encryption key, if zero is specified, the
		  *                default encryption key is used
		  * @param iv      initialization vector, if zero is specified,
		  *                the default initialization vector is used
		  * @return Returns MMRC_Ok if the activation was successful.
		  * @retval MMRC_Ok                   Successful activation
		  * @retval MMRC_MVC2_OutputNotFound  The output does not exist on
		  *                                   the card.                   
		*/
		TMmRc enableCinelink(const VideoOutput &output, uint32_t link, uint8_t key = 0, uint16_t iv = 0);
		
		/** @brief Enable CineLink(TM) 2 for a video output.
		  * Description
		  * This method enable the CineLine(TM) 2 for a given video
		  * \output. CineLink(TM) is a encryption algorithm for the
		  * HD-SDI transmission between the MVC card and the projector.
		  * CineLink(TM) 2 is the successor of CineLink(TM) 1 and is much
		  * more secure. It uses a better encryption algorithm and needs
		  * a TLS connection between the security manager and the
		  * projector for dynamic key change. In case of the
		  * SecurityAccess, which only works if the on board security
		  * manager is disabled, this dynamic key change must by done by
		  * the Player application.<p />
		  * This projector has a key storage which uses the key id as
		  * index. These means that a number of keys can be transferred
		  * to the projector and selected by the key id. This key id will
		  * be transmitted to the projector via the HD-SDI cable. A key
		  * id of zero is reserved and cannot be used.<p />
		  * Due to the support for multi link HD-SDI of the MVC card
		  * (e.g. dual HD-SDI). You have to specify the link where to
		  * enable the encryption.
		  * @param output     VideoOutput for which the encryption should
		  *                   be enabled 
		  * @param link       link number in the video output 
		  * @param key        128 bit (16 bytes) encryption key (big
		  *                   endian) 
		  * @param attr       64 bit (8 bytes) attribute of the key id
		  *                   (big endian) 
		  * @param keyid      key id which will be transmitted to the
		  *                   projector (12 bits used, zero is reserved) 
		  * @param nextkeyid  key id which will be used next, after the
		  *                   current key id (is also transmitted to the
		  *                   projector) 
		  * @return Returns MMRC_Ok on successful activation.
		  * @retval MMRC_Ok                   Successful activation 
		  * @retval MMRC_MVC2_OutputNotFound  The output does not exist on
		  *                                   the card.                   
		*/
		TMmRc enableCinelink2(const VideoOutput &output, uint32_t link, const uint8_t * key, const uint8_t * attr, uint16_t keyid, uint16_t nextkeyid);


		TMmRc setCineLink2Keys(CineLink2Para keys[4]);

		/** @brief Set media decryption key.
		  * Description
		  * The method is used to address the key storage for media
		  * decryption. The MVC card can decrypt input data using a
		  * AES128 engine. It has a space for 256 keys.<p />
		  * \See also <link mvc2::DataBuffer::setKeyIndex@uint16_t@uint8_t *@uint8_t *, DataBuffer::setKeyIndex()>.
		  * @param keyIndex  key index inside the key storage
		  * @param key       128 bit (16 byte) AES 128 key (big endian)
		  * @param type      media type, which will be decrypted with the
		  *                  key (needed for forensic marking)
		  * @param enableFM  enable the forensic marking if this key is
		  *                  used for decryption
		  * @return Returns MMRC_Ok if the key was successful stored.
		  * @retval MMRC_Ok  successful operation                                                                  
		*/
		TMmRc setMediaDecryptorKey(const uint16_t keyIndex, const uint8_t * key, const MediaType type, bool enableFM);

		/** @brief Enable forensic marking.
		  * Description
		  * The method enables the forensic marking for an output.
		  * Timecode and location information id will be inserted into
		  * the output. The timecode is a counter of 15 minutes starting
		  * from January the first of the current year. The firmware will
		  * not increase this timecode, a application must call this
		  * function every 15 minutes to increase the timecode. Location
		  * info id is a identifier for the cinema.
		  * @param output      VideoOutput to activate the forensic marking
		  *                    for 
		  * @param timecode    16 bit timecode to insert (15 minutes
		  *                    counter) 
		  * @param locationid  20 bit location info id 
		  * @return Returns MMRC_Ok on successful activation.
		  * @retval MMRC_Ok                   successful operation 
		  * @retval MMRC_MVC2_OutputNotFound  \output could not be found
		  *                                   on the card                  
		*/
		TMmRc enableForensicMarking(const VideoOutput &output, uint16_t timecode, uint32_t locationid);

		/** @brief Enable forensic marking.
		  * Description
		  * The method enables the forensic marking for an output.
		  * Timecode and location information id will be inserted into
		  * the output. The timecode is a counter of 15 minutes starting
		  * from January the first of the current year. The firmware will
		  * not increase this timecode, a application must call this
		  * function every 15 minutes to increase the timecode. Location
		  * info id is a identifier for the cinema.
		  * @param output      AudioOutput to activate the forensic marking
		  *                    for
		  * @param timecode    16 bit timecode to insert (15 minutes
		  *                    counter)
		  * @param locationid  20 bit location info id
		  * @return Returns MMRC_Ok on successful activation.
		  * @retval MMRC_Ok                   successful operation
		  * @retval MMRC_MVC2_OutputNotFound  \output could not be found
		  *                                   on the card                  
		*/
		TMmRc enableForensicMarking(const AudioOutput &output, uint16_t timecode, uint32_t locationid);

		/** @brief Disable forensic marking
		  * Description
		  * This method disables the forensic marking for an output.
		  * @param output  VideoOutput 
		  * @return Returns MMRC_Ok on success.
		  * @retval MMRC_Ok                   successful operation 
		  * @retval MMRC_MVC2_OutputNotFound  the output could not be
		  *                                   located                
		*/
		TMmRc disableForensicMarking(const VideoOutput &output);

		/** @brief Disable forensic marking
		  * Description
		  * This method disables the forensic marking for an output.
		  * @param output  AudioOutput
		  * @return Returns MMRC_Ok on success.
		  * @retval MMRC_Ok                   successful operation
		  * @retval MMRC_MVC2_OutputNotFound  the output could not be
		  *                                   located                
		*/
		TMmRc disableForensicMarking(const AudioOutput &output);

		TMmRc setForensicMarkingIndicator(const VideoOutput &output, bool enable);

		/* @brief Returns the security application version.
		Description
		Gets the security application version number.
		@return <link mvc2::VersionValue, VersionValue> object with
		the firmware version.
		*/
		TMmRc getFirmwareVersion(VersionValue &version) const;

		TMmRc getFipsFirmwareVersion(VersionValue &version) const;

		/** @brief Transfers data to target for sha256 hash calculation.
		 * Description
		 * This function is used to transfer data for sha256 hash calculation to target.
		 * Consecutive calls are possible before a call to getSha256HashValue() retrieves
		 * the output of the calculation, a sha256 hash value
		 * @param softwareId identfies the software that should calculate the hash value
		 * @param eHashAlg identifies the hash algorithm that should be used
		 * @param currentBufferIndex the index of current buffer that should be send to target,
		 *                           0 is the index of the first buffer, it initializes
		 *                           the algorithm at target
		 * @param buffer the data to be hashed, max buffer size depends on max packet size of underlying
		 *               transport, fragmentation is not supported
		 * @param bufferSize the size of data
		 * @return should be MMRC_Ok or MMRC_NotImplemented
		 */
		TMmRc setHashData(mvc2::SecurityAccess::SoftwareId softwareId, mvc2::SecurityAccess::HashAlgorithm eHashAlg,
			const int32_t currentBufferIndex,const int8_t* buffer, int32_t bufferSize, int32_t bufferLeftSize);


		/** @brief Retrieves the hash value from target
		 *	 @param hashBuffer when function returns MMRC_Ok this buffer contains the result of
		 *                    sha256 hash calculation
		 *  @return MMRC_Ok or MMRC_NotImplemented
		 */
		TMmRc getHashValue(mvc2::SecurityAccess::SoftwareId softwareId, mvc2::SecurityAccess::HashAlgorithm eHashAlg,uint8_t* hashValue,uint32_t* readSize)  const;

		TMmRc verify(mvc2::SecurityAccess::HashAlgorithm eHashAlg,uint8_t* signature,uint32_t u32SignatureSize,uint8_t* hashValue,uint32_t* readSize);

		/* @brief reads strings basing on the compiler defines __DATE__ and __TIME__
		@param moduleId the id of the module that should be checked
		@param date gets the date C string
		@param time gets the time C string
		*/
		TMmRc getBuildTimeStrings(uint32_t moduleId,char strDate[SecurityAccess::DateLen], char strTime[SecurityAccess::TimeLen]);

		/* @brief checks hardware conditions of RTC
		@param status gets status bit field,
		bit0..RTC hardware initialization (0..OK,1..Error)
		if bit0==1 following bits are "don't care"
		bit1..Power Loss (0..OK,1..Error Power was lost)
		bit2..Cover Button state(0..OK,1..Cover Button activated)
		bit3..CRC16 (0..OK,1..Mismatch)
		@return should be MMRC_Ok
		*/
		TMmRc getRtcHardwareStatus(uint32_t* status);
		TMmRc resetPowerfailBit(void);
		TMmRc hashRtcSram(void);

		/* @brief initializes the calculation of symmetric  encryption/decryption
		@param softwareId identifies the software that should encrypt or decrypt
		@param symmetricCryptoOp determines the crypto operation
		@param key the key that should be used, valid key size depends on algorithm
		@param iv the iv that should be used, valid iv size depends on algorithm, on those without iv, this parameter is ignored
		@return should be MMRC_Ok on success
		 */
		TMmRc symmetricCryptoInit(mvc2::SecurityAccess::SoftwareId softwareId, mvc2::SecurityAccess::SymmetricCryptoOperation symmetricCryptoOp, const uint8_t* key, const uint8_t* iv);

		/* @brief This function processes input data and gives the encrypted or decrypted result.
		@param softwareId the software that should process the input data
		@param counter of operation since last call to symmetricCryptoInit, should begin with 0
		@param inBuf input data (plain-text resp. cipher-text)
		@param inLen number of bytes that should be processed, this must not exceed 2048 and should be a multiple of according blocksize of algorithm, no padding inside
		@param outBuf this contains the processed data, output size is equal to input size
		*/
		TMmRc symmetricCryptoProcess(mvc2::SecurityAccess::SoftwareId softwareId, int32_t index,const uint8_t* inBuf, uint32_t inLen, uint8_t* outBuf );

		TMmRc symmetricCryptoFinish(mvc2::SecurityAccess::SoftwareId softwareId);

		TMmRc initHardwareInfo(void);
		TMmRc getAllHardwareInfo(uint32_t hwStatus[8], uint32_t hwValues[8] );
		TMmRc setHardwareInfo(uint32_t index,uint32_t mask,uint32_t value);
		TMmRc validateHardwareInfo(uint32_t index,uint32_t mask);

		TMmRc getAppletStatusCode(uint16_t& statusCode);
		TMmRc getAppletVersion(const int32_t appletId, uint32_t& major, uint32_t& minor, uint32_t& module, uint32_t& svnVersion);
		TMmRc rsaPrivateKeyDecCmp(char* plainData,uint32_t plainLen, char* cipherData,uint16_t& isoStatus);
		TMmRc selfTest(int32_t& status,uint16_t& isoStatus);
		TMmRc readRtcSram(uint32_t offset, uint8_t* value, bool askSm);
		TMmRc readRtcReg(uint32_t offset, uint8_t* value, bool askSm);
		TMmRc writeRtcReg(uint32_t offset, uint8_t value, bool askSm);

		

		/** This function reads the exponent and modulus of device specific MikroM public key.
		 * @param exponentResBuffer buffer to write exponent to
		 * @param exponentResBufferSize size of exponent buffer
		 * @param modulusResBuffer buffer to write modulus to
		 * @param modulusResBufferSize size of modulus buffer
		 * @param exponentResponseSize size of written exponent in exponent buffer
		 * @param modulusResponseSize  size of written modulus in modulus buffer
		 * @return  MMRC_Ok on success
		 *          MMRC_ArgumentOutOfRange when one of the buffer is to small
		 *          MMRC_MM_API_ReadSizeOutOfRange when response does not conform description in Tag MMTAG_SECURITY_DeviceKey
		 */
		TMmRc getMikromPublicKey(uint8_t* exponentResBuffer, size_t exponentResBufferSize, uint8_t* modulusResBuffer, size_t modulusResBufferSize,
			                      size_t* exponentResponseSize, size_t* modulusResponseSize);


		TMmRc initLogFlash(int32_t* initLogFlashRc,bool bMsgIf=false);

		/** This function reads hardware attributes of log flash device.
		 *  @param knownType [out] 0..this is know type, 1..this is unknown device
		 *  @param id [out] these five bytes contain the hardware id 
		 *  @param nomBytesPerPage [out] number of bytes per page
		 *  @param spareBytesPerPage [out] number of spare bytes per page
		 *  @param pagesPerBlock [out] pages per block
		 *  @param blocksPerDie [out] blocks per die
		 *  @param diesPerCE [out] dies per CE
		 *  @param cEsPerDevice [out] CEs per device 
		 */
		TMmRc readLogFlashAttributes( uint8_t* knownType, 
												uint8_t id[5],
												uint32_t* nomBytesPerPage ,
												uint32_t* spareBytesPerPage ,
												uint32_t* pagesPerBlock, 
												uint32_t* blocksPerDie,
												uint32_t* diesPerCE,
												uint32_t* cEsPerDevice);

		


		TMmRc setForensicMarkingId(uint8_t vendorID, uint16_t serial);
		
		/* @brief Reads the forensic marking id.
		* @param locationId  20 bit location info id 
		*/
		TMmRc getForensicMarkingId(uint32_t *locationId);

		/*@brief made to address and read an error code
		@param errorCodeVariableId address the variable that contains the error code with this id
		@param contains the read error code when function returns MMRC_Ok
		@param askSm true..forwarded to Security Application, false..forwarded to MIPS firmware
		@return MMRC_Ok on success
		*/
		TMmRc getErrorCode(int32_t errorCodeVariableId,int32_t *errorCode,bool askSm);

		/* @brief Reads the state of the FIPS self test. 
		 * @return MMRC_Ok self-test finished successfully
		 *         MMRC_SelftestError self-test ends with error
       *         MMRC_SelftestPending self-test in progress
		 */
		TMmRc getSelftestStatus(void);

		/*@brief Reads data from security interface.
		 @param address address to read from (0..255)
		 @param readData buffer to read data to
		 @param readDataSize number of bytes to read
		 @param readSize the number of read bytes
		 @return MMRC_Ok on success
		 */
		TMmRc readSecurityInterface(uint8_t address, uint8_t* readData, uint32_t readDataSize, uint32_t* readSize);

		/*@brief Writes data to security interface.
		@param address address to write to (0..255)
		@param writeData buffer to with data to write
		@param writeDataSize number of bytes to write
		@return MMRC_Ok on success
		*/
		TMmRc writeSecurityInterface(uint8_t address, uint8_t* writeData, uint32_t writeDataSize);



		TMmRc processSmartcardApdu(const uint8_t* cmd, size_t cmdSize, uint8_t* response, size_t responseBufSize, size_t* responseSize, int32_t* hardwareStatus, uint16_t* isoStatus);
		
		TMmRc getPublicKey(const int32_t keyPairId, uint8_t* modulusBuf, const size_t modulusBufSize, size_t* modulusSize, 
								 uint8_t* exponentBuf, const size_t expBufSize, size_t* expSize);


		TMmRc getCertificateSigningRequest(uint32_t certNumber, uint8_t *csrBuffer, uint32_t *csrBufferSize);

		TMmRc installCertificate(uint32_t certNumber, uint8_t *certBuffer, uint32_t certBufferSize);

		TMmRc deleteCertificateChain(uint32_t chainNumber);

		TMmRc getTamperStatus(TamperStatus &stat);

		/** @brief Check for object validity.
		  * Description
		  * This method is used to check if a object is filled with valid
		  * data and has there for a connection to a MvcDevice.          
		*/
		operator bool() const;

	private:
		SecurityAccess(MvcDevicePrivate *dev);

	private:
		MvcDevicePrivate *m_MvcDevicePrivate;
	};
}

#endif
