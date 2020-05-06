#ifndef _MVC2API_CONFIG_H_
#define _MVC2API_CONFIG_H_

#include "mvc2api.h"

namespace mvc2
{
	class FeatureAccessPrivate;

   class MVC2_API ConfigAccess
	{
	public:
		virtual ~ConfigAccess();

      ConfigAccess();
      ConfigAccess(TMmRc * resultPointer, const MvcDevice &mvcDevice);

      ConfigAccess(const ConfigAccess& other);
      ConfigAccess& operator=(const ConfigAccess& other);

      operator bool() const;

		TMmRc setMessageFifoSize(uint32_t size);
		TMmRc getMessageFifoSize(uint32_t *size);

		TMmRc setMessageFifoClients(uint32_t clients);
		TMmRc getMessageFifoClients(uint32_t *clients);

		TMmRc setLogFilter(uint32_t filter);
		TMmRc getLogFilter(uint32_t *filter);

		TMmRc setLogFilterConsole(uint32_t filter);
		TMmRc getLogFilterConsole(uint32_t *filter);

		TMmRc setNetworkDHCP(bool enable);
		TMmRc getNetworkDHCP(bool *enable);

		TMmRc setNetworkIPAddress(uint32_t ip);
		TMmRc getNetworkIPAddress(uint32_t *ip);

		TMmRc setNetworkMask(uint32_t mask);
		TMmRc getNetworkMask(uint32_t *mask);

	private:
		MvcDevicePrivate *m_DevicePrivate;
	};


	class MVC2_API FeatureAccess
	{
	public:
		virtual ~FeatureAccess();

		FeatureAccess();
		FeatureAccess(TMmRc * resultPointer, const MvcDevice &mvcDevice);

		FeatureAccess(const FeatureAccess& other);
		FeatureAccess& operator=(const FeatureAccess& other);

		operator bool() const;

		TMmRc queryFeature(const char *featureString);

		TMmRc getNextFeature(char *featureString, char *featureValue, uint32_t *validFrom, uint32_t *validThrough);

		TMmRc getFirstFeature(char *featureString, char *featureValue, uint32_t *validFrom, uint32_t *validThrough);

		TMmRc installFeature(const void * buffer, uint32_t buffersize);

	private:
		FeatureAccessPrivate *m_DevicePrivate;
	};
}
#endif
