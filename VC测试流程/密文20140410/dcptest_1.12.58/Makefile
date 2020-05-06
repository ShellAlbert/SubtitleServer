# 
CC=g++

CFLAGS= -Wall -Wno-switch -O3 -g
# CFLAGS= -Wall -Wno-switch -O3

# -DASDCP_PLATFORM="\"Linux i686\"" -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE

DEFS= -DASDCP_PLATFORM="\"Linux i686\"" -D_LARGEFILE_SOURCE -DPACKAGE_NAME=\"asdcplib\" -DPACKAGE_TARNAME=\"asdcplib\" -DPACKAGE_VERSION=\"1.12.58\" -DPACKAGE_STRING=\"asdcplib\ 1.12.58\" -DPACKAGE_BUGREPORT=\"asdcplib@cinecert.com\" -DPACKAGE=\"asdcplib\" -DVERSION=\"1.12.58\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -D_FILE_OFFSET_BITS=64 -DHAVE_LIBPTHREAD=1

CCFLAGS= -c

LDFLAGS= -G

COMPILE= $(CC) $(CFLAGS) $(CCFLAGS) $(INCLUDE_PARM) $(DEFS)

ARCHIVE= ar cr

# libasdcp include and src paths.
LIBASDCP_DIR=./dcptest_src/asdcplib-1.12.58
LIBASDCP_INCLUDE_DIR=$(LIBASDCP_DIR)/src
LIBASDCP_SRC_DIR=$(LIBASDCP_DIR)/src
MXFPARSERMODULE_DIR=./MXFParserModule
MXFPARSERMODULE_INCLUDE_DIR=$(MXFPARSERMODULE_DIR)
MXFPARSERMODULE_SRC_DIR=$(MXFPARSERMODULE_DIR)

INCLUDE_PARM= -I$(LIBASDCP_INCLUDE_DIR) \
	     -I$(MXFPARSERMODULE_INCLUDE_DIR) \
	                                 -I./

mxfparser_demo: \
	   libasdcp.a
	$(CC) $(CFLAGS) $(INCLUDE_PARM) $(DEFS) -lcrypto -o mxfparser_demo \
	                                                 main.cpp \
	                                             ./libasdcp.a


# Use -shared,if you want to create a *.so file.


libasdcp.a: \
		AS_DCP.o \
		AS_DCP_AES.o \
		AS_DCP_ATMOS.o \
		AS_DCP_DCData.o \
		AS_DCP_JP2K.o \
		AS_DCP_MPEG2.o \
		AS_DCP_MXF.o \
		AS_DCP_PCM.o \
		AS_DCP_TimedText.o \
		AtmosSyncChannel_Generator.o \
		AtmosSyncChannel_Mixer.o \
		CRC16.o \
		DCData_ByteStream_Parser.o \
		DCData_Sequence_Parser.o \
		Dict.o \
		h__Reader.o \
		h__Writer.o \
		Index.o \
		JP2K.o \
		JP2K_Codestream_Parser.o \
		JP2K_Sequence_Parser.o \
		KLV.o \
		KM_fileio.o \
		KM_log.o \
		KM_prng.o \
		KM_tai.o \
		KM_util.o \
		KM_xml.o \
		MDD.o \
		Metadata.o \
		MPEG.o \
		MPEG2_Parser.o \
		MXF.o \
		MXFTypes.o \
		PCM_Parser.o \
		PCMDataProviders.o \
		PCMParserList.o \
		SyncEncoder.o \
		TimedText_Parser.o \
		UUIDInformation.o \
		Wav.o \
		CMXFParser.o \
		CMXFParserModule.o
	$(ARCHIVE)            libasdcp.a \
	                        AS_DCP.o \
	                    AS_DCP_AES.o \
	                  AS_DCP_ATMOS.o \
	                 AS_DCP_DCData.o \
	                   AS_DCP_JP2K.o \
	                  AS_DCP_MPEG2.o \
	                    AS_DCP_MXF.o \
	                    AS_DCP_PCM.o \
	              AS_DCP_TimedText.o \
	    AtmosSyncChannel_Generator.o \
	        AtmosSyncChannel_Mixer.o \
	                         CRC16.o \
	      DCData_ByteStream_Parser.o \
	        DCData_Sequence_Parser.o \
	                          Dict.o \
	                     h__Reader.o \
	                     h__Writer.o \
	                         Index.o \
	                          JP2K.o \
	        JP2K_Codestream_Parser.o \
	          JP2K_Sequence_Parser.o \
	                           KLV.o \
	                     KM_fileio.o \
	                        KM_log.o \
	                       KM_prng.o \
	                        KM_tai.o \
	                       KM_util.o \
	                        KM_xml.o \
	                           MDD.o \
	                      Metadata.o \
	                          MPEG.o \
	                  MPEG2_Parser.o \
	                           MXF.o \
	                      MXFTypes.o \
	                    PCM_Parser.o \
	              PCMDataProviders.o \
	                 PCMParserList.o \
	                   SyncEncoder.o \
	              TimedText_Parser.o \
	               UUIDInformation.o \
	                           Wav.o \
	                    CMXFParser.o \
	              CMXFParserModule.o

AS_DCP.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AS_DCP.cpp

AS_DCP_AES.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AS_DCP_AES.cpp

AS_DCP_ATMOS.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AS_DCP_ATMOS.cpp

AS_DCP_DCData.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AS_DCP_DCData.cpp

AS_DCP_JP2K.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AS_DCP_JP2K.cpp

AS_DCP_MPEG2.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AS_DCP_MPEG2.cpp

AS_DCP_MXF.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AS_DCP_MXF.cpp

AS_DCP_PCM.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AS_DCP_PCM.cpp

AS_DCP_TimedText.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AS_DCP_TimedText.cpp

AtmosSyncChannel_Generator.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AtmosSyncChannel_Generator.cpp

AtmosSyncChannel_Mixer.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/AtmosSyncChannel_Mixer.cpp

CRC16.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/CRC16.c

DCData_ByteStream_Parser.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/DCData_ByteStream_Parser.cpp

DCData_Sequence_Parser.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/DCData_Sequence_Parser.cpp

Dict.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/Dict.cpp

h__Reader.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/h__Reader.cpp

h__Writer.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/h__Writer.cpp

Index.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/Index.cpp

JP2K.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/JP2K.cpp

JP2K_Codestream_Parser.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/JP2K_Codestream_Parser.cpp

JP2K_Sequence_Parser.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/JP2K_Sequence_Parser.cpp

KLV.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/KLV.cpp

KM_fileio.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/KM_fileio.cpp

KM_log.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/KM_log.cpp

KM_prng.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/KM_prng.cpp

KM_tai.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/KM_tai.cpp

KM_util.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/KM_util.cpp

KM_xml.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/KM_xml.cpp

MDD.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/MDD.cpp

Metadata.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/Metadata.cpp

MPEG.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/MPEG.cpp

MPEG2_Parser.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/MPEG2_Parser.cpp

MXF.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/MXF.cpp

MXFTypes.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/MXFTypes.cpp

PCM_Parser.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/PCM_Parser.cpp

PCMDataProviders.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/PCMDataProviders.cpp

PCMParserList.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/PCMParserList.cpp

SyncEncoder.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/SyncEncoder.c

TimedText_Parser.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/TimedText_Parser.cpp

UUIDInformation.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/UUIDInformation.c

Wav.o:
	$(COMPILE) -I$(LIBASDCP_INCLUDE_DIR) $(LIBASDCP_SRC_DIR)/Wav.cpp

CMXFParser.o:
	$(COMPILE) $(INCLUDE_PARM) $(MXFPARSERMODULE_SRC_DIR)/CMXFParser.cpp

CMXFParserModule.o:
	$(COMPILE) $(INCLUDE_PARM) $(MXFPARSERMODULE_SRC_DIR)/CMXFParserModule.cpp


clean:
	rm -f core *.o *.a
