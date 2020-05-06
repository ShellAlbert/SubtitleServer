#ifndef _MVC2API_DEVICE_H_
#define _MVC2API_DEVICE_H_

#include "mvc2api_types.h"
#include <new>
// for time_t
#include <ctime>

namespace mvc2
{
	class MvcDeviceIteratorPrivate;
	class MvcDevicePrivate;
	class SystemStatusPrivate;
	class MvcNetDeviceIteratorPrivate;
	class CPUInfoPrivate;


	/* Class to hold version numbers */
	class MVC2_API VersionValue
	{
	public:
		/* @brief Default constructor
		Description
		Constructor which creates a VersionValue object with the
		\version number given as 32bit value.
		@param version  \version number, big endian (upper byte is
		\version, lower byte is build revision)    */
		VersionValue(uint32_t version = 0)
		{
			m_version = (version>>24) & 0xff;
			m_revision = (version>>16) & 0xff;
			m_build_ver = (version>>8) & 0xff;
			m_build_rev = (version) & 0xff;
		}

		/** @brief Copy constructor.
		  * Description
		  * This constructor creates a new VersionValue object with a
		  * value of the source object.
		  * @param inver  source VersionValue object                 
		*/
		VersionValue(const VersionValue &inver)
		{
			m_version = inver.m_version;
			m_revision = inver.m_revision;
			m_build_ver = inver.m_build_ver;
			m_build_rev = inver.m_build_rev;
		}

		/* @brief Constructor with version number as bytes
		Description
		Create a VersionValue object based on given version
		information.
		@param ver       \version number
		@param rev       \version revision
		@param buildver  build number
		@param buildrev  build revision                     */
		VersionValue(uint32_t ver, uint32_t rev, uint32_t buildver, uint32_t buildrev)
		{
			m_version = ver;
			m_revision = rev;
			m_build_ver = buildver;
			m_build_rev = buildrev;
		}

		/* @brief Return version
		Description
		Gets version number.
		@return Version number.
		*/
		uint32_t getVersion() const
		{
			return(m_version);
		}

		/* @brief Return revision
		Description
		Gets revision number.
		@return Revision number.
		*/
		uint32_t getRevision() const
		{
			return(m_revision);
		}

		/* @brief Return build version.
		Description
		Gets build number.
		@return Build number.
		*/
		uint32_t getBuildVersion() const
		{
			return(m_build_ver);
		}

		/* @brief Return build revision.
		Description
		Gets build revision number.
		@return Build revision.       */
		uint32_t getBuildRevision() const
		{
			return(m_build_rev);
		}

		/* @brief Default assignment operator
		Description
		Copies all information from a source VersionValue object to a
		new one.
		@param inver  \input VersionValue object
		@return True of the new VersionValue object is not empty.
		*/
		VersionValue &operator =(const VersionValue &inver)
		{
			m_version = inver.m_version;
			m_revision = inver.m_revision;
			m_build_ver = inver.m_build_ver;
			m_build_rev = inver.m_build_rev;
			return(*this);
		}

		/** @brief Bool operator.
		  * Description
		  * This operator is used to check if a VersionValue is not zero.
		*/
		operator bool() const
		{
			return((m_version != 0) || (m_revision != 0) || (m_build_ver != 0) || (m_build_rev != 0));
		}

	private:
		uint32_t m_version;
		uint32_t m_revision;
		uint32_t m_build_ver;
		uint32_t m_build_rev;
	};


	/* This class holds the collected information about the current
	   system status. It is the result of the
	   MvcDevice::getSystemStatus() method.                         */
	class MVC2_API SystemStatus
	{
	public:
		/* CPU load enumeration used for getCPULoad() method. */
		enum CPULOAD
		{
			Load_100ms, /* CPU load calculated over a 100 ms period of time.  */
			
			Load_1s, /* CPU load calculated over a 1 second period of time. */
			
			Load_10s /* CPU load calculated over a 10 seconds period of time. */
		};

	public:
		/* @brief Default constructor.
		   Description
		   This constructor produces a empty SystemStatus object with
		   any value set to zero.                                     */
		SystemStatus();

		SystemStatus(const SystemStatus &other);

		SystemStatus(MvcDevicePrivate *mvcdev, bool readTemperatures);

		~SystemStatus();

		/* @brief Gets the CPU load information.
		   Description
		   This method will return the CPU load of the firmware for a
		   given time span.
		   @param load  period of time (see enumerations) of the CPU load
		                calculation
		   @return CPU load in percent (value between 0 and 100, where
		   100 means maximal CPU load)
		                                                                  */
		uint32_t getCPULoad(CPULOAD load) const;

		/* @brief Gets used memory information from the firmware.
		   Description
		   This method returns the number of bytes used on the firmware.
		   @return Number of bytes used in the firmware.
		                                                                 */
		uint32_t getUsedMemory() const;

		/* @brief Gets the memory information of the firmware.
		   Description
		   This method will return the total bytes of memory from the
		   firmware.
		   @return Total number of memory bytes.
		                                                              */
		uint32_t getMaxMemory() const;

		/* @brief Gets free memory information from the firmware.
		   Description
		   This method returns the number of bytes used on the firmware.
		   @return Number of bytes free on the firmware.                 */
		uint32_t getFreeMemory() const;

		float getOutputTemperature() const;

		float getDecoderTemperature() const;

		float getBoardTemperature() const;

		uint32_t getFanSpeed() const;

		// values 0-100 or -1 if not available
		int32_t getBatteryStatus() const;

		/* @brief Assignment operator.
		Description
		The operator copies all data from a source object.
		@param inbuf  source SystemStatus object
		@return Returns true if the new SystemStatus object is not
		empty.
		*/
		SystemStatus &operator = (const SystemStatus &inbuf);

		operator bool() const;

	private:
		SystemStatusPrivate *m_SystemStatusPriv;
	};


	class MVC2_API CPUInfo
	{
	public:
		enum CPUTYPE
		{
			CPUTYPE_Unknown = 0,
			CPUTYPE_PMCSierra_MSP8510 = 1,
		};

	public:
		CPUInfo();
		CPUInfo(MvcDevicePrivate *mvcdev, uint32_t index);
		CPUInfo(const CPUInfo &other);
		~CPUInfo();

		CPUTYPE getCPUType() const;
		const char *getCPUTypeString() const;
		uint32_t getNumberOfCores() const;
		VersionValue getHardwareVersion() const;
		uint64_t getCoreFrequency() const;
		uint32_t getCoreFrequencykHz() const;

		CPUInfo &operator = (const CPUInfo &inbuf);

		operator bool() const;

	private:
		CPUInfoPrivate *m_CPUInfoPriv;
	};


	/* Network interface information class */
	class MVC2_API NetworkInterfaceInfo
	{
	public:
		NetworkInterfaceInfo();

		NetworkInterfaceInfo(const NetworkInterfaceInfo &ininfo);
		NetworkInterfaceInfo(const uint32_t ipaddr, const uint8_t * mac = 0);

		/* @brief Return the IP address
		Description
		Gets the IP address as a 32 bit value.
		@return IP address
		*/
		uint32_t getIPAddress() const;

		/* @brief Return IP address
		Description
		Gets the IP address as four 8bit values.
		@param ipaddr  Array of bytes which receives 4 bytes of the IP
		address                                         */
		void getIPAddress(uint8_t *ipaddr) const;

		/* @brief Return MAC address
		Description
		Gets the MAC address.
		@param mac  pointer to an array which receives the 6 bytes of
		MAC address                                       */
		void getMACAddress(uint8_t *mac) const;

		NetworkInterfaceInfo &operator =(const NetworkInterfaceInfo &innet);

		operator bool() const;

	private:
		uint32_t m_ipaddr;
		uint8_t m_mac[6];
	};


	/** Description
	  * This class is used to access a MVC20x media block. Objects of
	  * this class cannot be directly created, you need to use a <link mvc2::MvcDeviceIterator, MvcDeviceIterator>
	  * \or <link mvc2::MvcNetDeviceIterator, MvcNetDeviceIterator>.                                              
	*/
	class MVC2_API MvcDevice
	{
	public:
		enum OUTPUTMODE
		{
			OutputMode_SDI									= 0,
			OutputMode_DVI									= 1
		};

		enum CLOCKID
		{
			ClockId_SM										= 0,
			ClockId_Firmware								= 1,
			ClockId_System									= 2
		};

		enum PRODUCTCODE
		{
			ProductCode_MVC200DC							= 2224,
			ProductCode_MVC201							= 2250
		};

	public:
		/* @brief Default constructor.
		Description
		The default constructor will generate a MvcDevice object
		without any information about a real MVC card.           */
		MvcDevice();

		MvcDevice(const MvcDevice &indev);

		/* @brief MvcDevice destructor.
		Description
		This is the generic MvcDevice destructor, which will free any
		resources allocated by the MvcDevice.                         */
		~MvcDevice();

		/* @brief Assignment operator.
		Description
		This operator will copy all information from the incoming <link mvc2::MvcDevice, MvcDevice>
		and frees the resources which were used be the <link mvc2::MvcDevice, MvcDevice>
		before.
		@param mvcdev  \Input MvcDevice to copy data from.
		@return true will be returned if the input <link mvc2::MvcDevice, MvcDevice>
		is not NULL, which means it actual holds information about a
		MVC card.
		*/
		MvcDevice &operator =(const MvcDevice &mvcdev);


		/* @brief Get UID.
		Description
		After a MVC card is opened with the <link mvc2::MvcDeviceIterator, MvcDeviceIterator>, this 
		functions return code should be checked for valid device state.
		@return TMmRc device state
		@retval MMRC_MVC2_OpenFailed								CreateFile could not open device
		@retval MMRC_MVC2_WrongDriverInterfaceVersion		API and driver are not compatible
		@retval MMRC_MVC2_BadPowerSupply							power supply is not connected
		@retval MMRC_MVC2_BootImageNotLoaded					basic bootup procedure failed
		@retval MMRC_Ok												device is up and running
		*/
		TMmRc getDeviceState() const;

		/** @brief Get UID.
		  * Description
		  * Every MVC card has a unique serial number. This method will
		  * return this number.
		  * @return 32 bit unique identifier of the card. Zero will be
		  * returned if an error happened.                             
		*/
		uint32_t getUID() const;

		/* @brief Returns the firmware version.
		Description
		Gets the firmware version number.
		@return <link mvc2::VersionValue, VersionValue> object with
		the firmware version.
		*/
		VersionValue getFirmwareVersion() const;

		/* @brief Return driver version.
		Description
		Gets the version of the used driver.
		@return <link mvc2::VersionValue, VersionValue> object with
		the driver version.
		*/
		VersionValue getDriverVersion() const;

		/* @brief Return API version
		Description
		Gets the version of the API dll.
		@return <link mvc2::VersionValue, VersionValue> object with
		the API version.
		*/
		VersionValue getAPIVersion() const;

		/* @brief Return version of the card bootloader.
		@return <link mvc2::VersionValue, VersionValue> object with
		the bootloader version.
		*/
		VersionValue getBootloaderVersion() const;

		/* @brief Return version of the cards security bootloader.
		@return <link mvc2::VersionValue, VersionValue> object with
		the security bootloader version.
		*/
		VersionValue getSecurityBootloaderVersion() const;

		/* @brief Return version of the cards security manager.
		@return <link mvc2::VersionValue, VersionValue> object with
		the security manager version.
		*/
		VersionValue getSecurityManagerVersion() const;

		/* @brief Return IP server version.
		Description
		Gets the version of the IP server if connected via IP.
		@return <link mvc2::VersionValue, VersionValue> object with
		the IP server version.
		*/
		VersionValue getIPServerVersion() const;

		/* @brief Return network information
		Description
		Gets network information of the device. The MVC
		card driver provides a virtual network driver which can be used to
		communicate with the device.
		@return <link mvc2::NetworkInterfaceInfo, NetworkInterfaceInfo>
		\object, which is empty if no information could be found.
		*/
		NetworkInterfaceInfo getNetworkConfiguration() const;

		/* @brief Return product code
		Description
		\Returns the product code of the MVC card to identify the
		card type.
		@return Product code number.
		*/
		uint32_t getProductCode() const;
		/* @brief Return product revision
		Description
		Gets product revision number, were one is the first revision.
		@return Product revision number.
		*/
		uint32_t getProductRevision() const;

		/* @brief Return PCI slot number
		Description
		Gets the PCI slot number of the MVC card.
		@return PCI slot number.
		*/
		uint32_t getPCISlot() const;
		/* @brief Return PCI bus number.
		Description
		Gets the PCI bus number of the MVC card.
		@return PCI bus number.
		*/
		uint32_t getPCIBus() const;

		/* @brief Gets the system status.
		   Description
		   The method will return useful information about the status of
		   the firmware.
		   @return SystemStatus object with the collected information.
		   The SystemStatus object will be empty (filled with zeros) if
		   an error happened.
		                                                                 */
		SystemStatus getSystemStatus(bool readTemperatures = true) const;

		/* @brief Internal function to get the private object.
		   Description
		   This method is used only internally and should not be used by
		   any application!
		   @return Device private object.
		                                                                 */
		MvcDevicePrivate *getDevicePrivate() const
		{
			return(m_DevicePriv);
		}

		TMmRc resetCard();

		/* @brief Ingest a log message in the card log.
		Description
		This method will ingest a log message in the cards log.
		@param severity severity of the message (use LogMessage::Severity)
		@param subid sub id of the message
		@param msg ansi string to ingest
		@param msgsize size in bytes of the message including string termination
		@return MMRC_Ok or an error.
		*/
		TMmRc putMessage(uint32_t severity, uint32_t subid, char *msg, uint32_t msgsize);

		/* @brief Reads the posix time.
			@param clockId select the clock to read out, 0 for SM and 1 for firmware
			@return posix time or -1 on error	
		*/
		time_t getSystemPosixTime(CLOCKID clockId = ClockId_SM);

		/** @brief Reads the uptime in milliseconds.
		  * @param clockId  select the clock to read out, 0 for SM, 1 for
		  *                 firmware, 2 for local OS 
		  * @return uptime in milliseconds                               
		*/
		uint64_t getUptimeMs(CLOCKID clockId = ClockId_SM);
		
		TMmRc getLastError() const;

		TMmRc setOutputMode(OUTPUTMODE mode);

		TMmRc setAuthenticationPassword(const char *passwd);

		TMmRc setPowerDownTime(uint32_t seconds);

		TMmRc getPowerDownStatus(int32_t *powerState, int32_t *powerDownTime, int32_t *timeToPowerDown);

		/** @brief Reads the crc16 checksum of security application.
		  * @param checksum crc16 calculated over internal program flash
		  * @return MMRC_Ok on success with valid checksum
       */
		TMmRc getSecurityManagerHwStatus(uint16_t* secAppCrc16, int32_t* mainFpgaProgRc, int32_t* outFpgaProgRc, int32_t* rtcInitRc);

		/** @brief Get CPU information.
		  * Description
		  * Get basic information about the CPUs used on the MediaBlock.
		  * It could be useful to find out some differences on board
		  * revisions.
		  * @param cpuIndex  index of the CPU
		  * @return Returns a CPUInfo object.<p />                      
		*/
		CPUInfo getCPUInfo(uint32_t cpuIndex) const;

		TMmRc setStatusLed(uint32_t which, uint32_t color, uint32_t blinkmask);

		static const char *getErrorString(TMmRc rc);
		static const char *getErrorDescription(TMmRc rc);

		operator bool() const;

	private:
		MvcDevice(MvcDevicePrivate *devpriv);

	private:
		/* Pointer to the MvcDevicePrivate object which holds the actual
		functions.                                                    */
		MvcDevicePrivate *m_DevicePriv;

#ifndef MVC2API_NETWORK_ONLY
		friend class MvcDeviceIterator;
#endif
		friend class MvcNetDeviceIterator;
	};

#ifndef MVC2API_NETWORK_ONLY

	/* \ \ 
	@brief MVC20x device iterator class.


	Description
	This class is used to parse the MVC20x cards available on
	the system. It holds a list of cards which can be parsed with
	<link mvc2::MvcDeviceIterator::getNext, getNext()> method.    */
	class MVC2_API MvcDeviceIterator
	{
	public:
		/* @brief Default constructor.
		Description
		The list of MVC cards will be initialized. To get the first
		MVC card found, use the <link mvc2::MvcDeviceIterator::getNext, getNext()>
		method.                                                                    */
		MvcDeviceIterator();

		MvcDeviceIterator(const MvcDeviceIterator &initor);


		/* @brief Default destructor.
		Description
		All resources which were need to parse MVC card list will be
		freed.                                                       */
		~MvcDeviceIterator();


		/* @brief Gets next MVC device.
		Description
		This method is used to get the next <link mvc2::MvcDevice, MvcDevice>
		for the iterator. If there's no more MVC card found an empty <link mvc2::MvcDevice, MvcDevice>
		will be returned.
		@return An <link mvc2::MvcDevice, MvcDevice> object.
		*/
		MvcDevice getNext();

		/* @brief Gets first MvcDevice.
		   Description
		   This method returns the first MvcDevice found on the
		   computer, if there is one.
		   @return A MvcDevice. If an error occurred, the MvcDevice will
		   be empty.
		                                                                 */
		MvcDevice getFirst();

		/* @brief Get MvcDevice by index.
		   Description
		   The iterator will return the MvcDevice at a specific index.
		   It will also set the iterator at that position, so getNext()
		   can be used to continue parsing. It's recommended to use
		   getNext() method instead of getIndex().
		   @param index  card index starting by zero
		   @return A MvcDevice. If an error occurred, the MvcDevice will
		   be empty.
		                                                                 */
		MvcDevice getIndex(uint32_t index);

	private:
		MvcDeviceIteratorPrivate *m_DeviceItorPriv;
	};

#endif

	class MVC2_API MvcNetDeviceIterator
	{
	public:
		MvcNetDeviceIterator(const char *hostname, const char *hostname_ctrl = 0);
		MvcNetDeviceIterator(const MvcNetDeviceIterator &initor);

		~MvcNetDeviceIterator();

		MvcDevice getNext();
		MvcDevice getFirst();
		MvcDevice getIndex(uint32_t index);

	private:
		MvcNetDeviceIteratorPrivate *m_DeviceItorPriv;
	};
}


#endif /* _MVC2API_DEVICE_H_ */
