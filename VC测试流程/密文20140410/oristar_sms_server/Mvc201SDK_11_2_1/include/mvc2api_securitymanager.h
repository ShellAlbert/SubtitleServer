#ifndef _MVC2API_SECURITYMANAGER_H_
#define _MVC2API_SECURITYMANAGER_H_

#include "mvc2api_device.h"

namespace mvc2
{
	class SecurityManagerPrivate;
	class MvcDecoder;

	/** @brief Helper class for UUIDs.
	  * Description
	  * This class provides an easy interface to work with 128 bit
	  * UUIDs.                                                    
	*/
	class MVC2_API UuidValue
	{
	public:
		UuidValue(const uint8_t *key);
		UuidValue(uint8_t fill = 0);
		UuidValue(const UuidValue &inkey);
		UuidValue(uint8_t k0,uint8_t k1,uint8_t k2,uint8_t k3,
			uint8_t k4,uint8_t k5,uint8_t k6,uint8_t k7,
			uint8_t k8,uint8_t k9,uint8_t k10,uint8_t k11,
			uint8_t k12,uint8_t k13,uint8_t k14,uint8_t k15);
		UuidValue(const char *str);

		bool operator ==(const UuidValue &inkey) const;
		bool operator !=(const UuidValue &inkey) const;
		UuidValue &operator =(const UuidValue &inkey);

		uint8_t operator[](uint8_t index) const
		{
			return(m_key[index]);
		}

		uint8_t &operator[](uint8_t index)
		{
			return(m_key[index]);
		}

		operator bool() const;

		char *toString(char *stringBuffer) const;

		void toArray(uint8_t *array) const;

	protected:
		uint8_t m_key[16];
	};


	/** @brief Security manager API implementation.
	  * Description
	  * This class provides an interface to the MediaBlock internal
	  * security manager. The connection to the security manager must
	  * be save, that's why it implements a TLS connection for the
	  * communication. Therefore a simple object create is not enough
	  * here and a TLS connection handshaking must be done before
	  * sending any security manager commands. To establish the
	  * connection a SMS certification chain is needed which fits to
	  * the DCI certification chain installed on the IMB. First setup
	  * the chain using addCertificate()/loadCertificateChainFile()
	  * and usePrivateKey()/loadPrivateKeyFile(). After that call
	  * connect() and the TLS connection will be established. Any
	  * security manager function can be used now. The TLS connection
	  * will automatically be closed on deletion of the object.      
	*/
	class MVC2_API SecurityManager
	{
	public:
		typedef struct
		{
			uint8_t b3;
			uint8_t b2;
			uint8_t b1;
			uint8_t b0;  /* as in b0.b1.b2.b3 */
		} SpbIpAddress;

		enum CPL_ASSET_EXCEPTION
		{
			CplException_None				= 0,
			CplException_Hash				= 1,
			CplException_Missing			= 2
		};
		
		enum SM_OPERATION
		{
			Operation_Unknown						= 0,
			Operation_NotPlaying					= 1,		// idle
			Operation_Playing						= 2
		};

		enum SUITE_STATUS
		{
			Suite_Started		= 1,
			Suite_Stopped		= 2,
			Suite_Progress		= 3,
			Suite_Restarted	= 4,
			Suite_Lost			= 5,
			Suite_Error			= 6
		};

		static const uint32_t c_ProjectorCertificate = 10000;

		typedef struct 
		{
			UuidValue key_id;
			UuidValue kdm_id;
			uint16_t key_index;
			uint8_t key_type;
		} KeyMap;

		typedef struct 
		{
			UuidValue key_id;
			uint8_t aes_key[16];
		} AesKeyMap;

	public:
		virtual ~SecurityManager();

		SecurityManager();
		SecurityManager(TMmRc * resultPointer, const MvcDevice &mvcDevice);
		SecurityManager(TMmRc * resultPointer, const MvcDevice &mvcDevice, int32_t timeZoneOffsetMins);

		SecurityManager(const SecurityManager& other);
		SecurityManager& operator=(const SecurityManager& other);

		/** @brief Setup security manager connection.
		  * Description
		  * After certificate chain and private key setup a TLS
		  * connection can be established. Connect() will do all the
		  * necessary work with the handshaking and result ideally in a
		  * successful established TLS connection.
		  * @return MMRC_Ok on success.                                
		*/
		TMmRc connect();

		/** @brief Start the security manager suite.
		  * Description
		  * The security manager suite will be started and all connected
		  * SPBs will be saved. The SPBs will be used later to check
		  * against the trusted device list. Normally you should provide
		  * \one or more projector ip addresses here. In case of a MVC201
		  * the list can be empty, then the MediaBlock will use the
		  * \internal projector connection.
		  * @param spbList   pointer to a array of ip addresses
		  * @param spbCount  number of entries of spbList
		  * @return MMRC_Ok will be return if the suite was started
		  * successfully, else an error.                                 
		*/
		TMmRc startSuite(SpbIpAddress *spbList, uint32_t spbCount);

		/** @brief Start the security manager suite.
		  * Description
		  * The security manager suite will be started and all connected
		  * SPBs will be saved. The SPBs will be used later to check
		  * against the trusted device list. Normally you should provide
		  * \one or more projector ip addresses here. In case of a MVC201
		  * the list can be empty, then the MediaBlock will use the
		  * \internal projector connection.
		  * @param spbList  string with multiple ip addresses separated by
		  *                 ',' or '/'
		  * @return MMRC_Ok on successful operation.                      
		*/
		TMmRc startSuite(const char *spbList = 0);

		TMmRc stopSuite();

		TMmRc uploadCplFile(const char *cplPath, CPL_ASSET_EXCEPTION assetExc = CplException_None);

		TMmRc uploadCplBuffer(const uint8_t *cplBuffer, uint32_t bufferSize, CPL_ASSET_EXCEPTION assetExc = CplException_None);

		TMmRc uploadKdmFile(const char *kdmPath, AesKeyMap *subKeyArray = 0, uint32_t *numberOfEntries = 0);

		TMmRc uploadKdmBuffer(const uint8_t *kdmBuffer, uint32_t bufferSize, AesKeyMap *subKeyArray = 0, uint32_t *numberOfEntries = 0);

		TMmRc purgeCpl(const UuidValue &cplUuid);

		TMmRc getCplList(UuidValue *cplUuidArray, uint32_t *arrayLen);

		TMmRc getKdmList(const UuidValue &cplUuid, UuidValue *kdmUuidArray, uint32_t *arrayLen);

		/** @brief Prepare and activate a show.
		  * Description
		  * This function will prepare and activate a should which
		  * consists of multi CPLs. All CPLs and the according KDMs must
		  * be uploaded before using this function. The keys will be
		  * activated and the key uuid to key index translation table
		  * will be saves inside the security manager object. The
		  * automatic key index translation is used if the security
		  * manager object is set in the decoder (using <link mvc2::MvcDecoder::setSecurityManager@SecurityManager &amp;, MvcDecoder::setSecurityManager Method>).
		  * The function also returns the maximum time the show is valid.
		  * This valid has only a informational purpose and can be used
		  * by the SMS to inform the user.
		  * @param authId        authentication identifier used for logs
		  * @param cplUuidArray  array of CPL UUIDs used by the show
		  * @param arrayLen      number of entries of cplUuidArray
		  * @param keyExpTime    pointer to a variable which receives the
		  *                      minimum expire time of the show
		  * @return MMRC_Ok if the show was activated, otherwise an
		  * error.                                                                                                                                                
		*/
		TMmRc playShow(const char *authId, const UuidValue *cplUuidArray, uint32_t arrayLen, uint64_t *keyExpTime = 0);

		TMmRc stopShow(const char *authId);

		TMmRc checkShow(const UuidValue *cplUuidArray, uint32_t arrayLen, uint64_t checkTime, uint64_t *keyExpTime = 0);

		TMmRc adjustTime(const char *authId, int32_t diff);

		TMmRc getCertificate(uint32_t which, uint8_t *buffer, uint32_t *buffersize);

		TMmRc queryStatus(SM_OPERATION *smOp, SUITE_STATUS *suiteStat, uint64_t *secureTime, int32_t *time_adjust);

		TMmRc getLogReport(uint8_t *buffer, uint32_t *buffersize, uint64_t startTime, uint64_t endTime, uint64_t *lastLogTime);

		TMmRc initiateMarriage(const char *authId);

		TMmRc serviceDoorTamperTermination(const char *authId);

		TMmRc getKeyMap(KeyMap *keymapArray, uint32_t *numberOfEntries);

		/** @brief Build a SMS certification chain.
		  * Description
		  * This method adds a new certificate to create a complete chain
		  * which will be used to open the TLS connection to the security
		  * manager. The first certificate is the SMS certificate, where
		  * a private key is also needed (see <link mvc2::SecurityManager::usePrivateKey@void *, usePrivateKey>).
		  * The second certificate is an intermediate and the last one
		  * the root certificate.
		  * @param cert  pointer to a X509 structure with the certificate
		  * @return MMRC_Ok on success.                                                                          
		*/
		TMmRc addCertificate(void *cert);
		
		/** @brief Set private to use within the TLS connection.
		  * Description
		  * After setting up the certificate chain for the TLS
		  * connection, the private key for the SMS is needed. This
		  * function is used to set it.
		  * @param pkey  pointer to a EVP_PKEY with the private key
		  * @return MMRC_Ok on success.                            
		*/
		TMmRc usePrivateKey(void *pkey);

		TMmRc loadCertificateChainFile(const char *filename);
		TMmRc loadPrivateKeyFile(const char *filename);

		TMmRc zeroize(const char *authId);


		/** @brief Check for object validity.
		  * Description
		  * This method is used to check if a object is filled with valid
		  * data and has there for a connection to a MvcDevice.          
		*/
		operator bool() const;

	private:
		SecurityManagerPrivate *getPrivate() const;
		SecurityManagerPrivate *m_SMPrivate;

		friend class MvcDecoder;
	};
}

#endif
