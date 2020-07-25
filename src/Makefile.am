# Copyright (c) 2013-2016 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

DIST_SUBDIRS = secp256k1 univalue

AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS) $(HARDENED_LDFLAGS) $(GPROF_LDFLAGS) $(SANITIZER_LDFLAGS)
AM_CXXFLAGS = $(DEBUG_CXXFLAGS) $(HARDENED_CXXFLAGS) $(WARN_CXXFLAGS) $(NOWARN_CXXFLAGS) $(ERROR_CXXFLAGS) $(GPROF_CXXFLAGS) $(SANITIZER_CXXFLAGS)
AM_CPPFLAGS = $(DEBUG_CPPFLAGS) $(HARDENED_CPPFLAGS)
AM_LIBTOOLFLAGS = --preserve-dup-deps
EXTRA_LIBRARIES =

if EMBEDDED_UNIVALUE
LIBUNIVALUE = univalue/libunivalue.la

$(LIBUNIVALUE): $(wildcard univalue/lib/*) $(wildcard univalue/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
else
LIBUNIVALUE = $(UNIVALUE_LIBS)
endif

ALQO_INCLUDES=-I$(builddir) $(BDB_CPPFLAGS) $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

ALQO_INCLUDES += -I$(srcdir)/secp256k1/include
ALQO_INCLUDES += $(UNIVALUE_CFLAGS)

LIBALQO_SERVER=libalqo_server.a
LIBALQO_COMMON=libalqo_common.a
LIBALQO_CLI=libalqo_cli.a
LIBALQO_UTIL=libalqo_util.a
LIBALQO_CRYPTO=crypto/libalqo_crypto.a
LIBALQOQT=qt/libalqoqt.a
LIBSECP256K1=secp256k1/libsecp256k1.la

if ENABLE_ZMQ
LIBALQO_ZMQ=libalqo_zmq.a
endif
if BUILD_ALQO_LIBS
LIBALQOCONSENSUS=libalqoconsensus.la
endif
if ENABLE_WALLET
LIBALQO_WALLET=libalqo_wallet.a
endif

$(LIBSECP256K1): $(wildcard secp256k1/src/*.h) $(wildcard secp256k1/src/*.c) $(wildcard secp256k1/include/*)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

# Make is not made aware of per-object dependencies to avoid limiting building parallelization
# But to build the less dependent modules first, we manually select their order here:
EXTRA_LIBRARIES += \
  $(LIBALQO_CRYPTO) \
  $(LIBALQO_UTIL) \
  $(LIBALQO_COMMON) \
  $(LIBALQO_SERVER) \
  $(LIBALQO_CLI) \
  $(LIBALQO_WALLET) \
  $(LIBALQO_ZMQ)

lib_LTLIBRARIES = $(LIBALQOCONSENSUS)

bin_PROGRAMS =
noinst_PROGRAMS =
TESTS =
BENCHMARKS =

if BUILD_ALQOD
  bin_PROGRAMS += alqod
endif

if BUILD_ALQO_UTILS
  bin_PROGRAMS += alqo-cli alqo-tx
endif

.PHONY: FORCE check-symbols check-security
# alqo core #
ALQO_CORE_H = \
  activemasternode.h \
  addrman.h \
  alert.h \
  allocators.h \
  amount.h \
  base58.h \
  bip38.h \
  bloom.h \
  blocksignature.h \
  chain.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  coins.h \
  compat.h \
  compat/byteswap.h \
  compat/endian.h \
  compat/sanity.h \
  compressor.h \
  primitives/block.h \
  primitives/transaction.h \
  core_io.h \
  crypter.h \
  pairresult.h \
  addressbook.h \
  obfuscation.h \
  obfuscation-relay.h \
  wallet/db.h \
  hash.h \
  httprpc.h \
  httpserver.h \
  init.h \
  kernel.h \
  swifttx.h \
  key.h \
  keystore.h \
  leveldbwrapper.h \
  limitedmap.h \
  main.h \
  masternode.h \
  masternode-payments.h \
  masternode-budget.h \
  masternode-sync.h \
  masternodeman.h \
  masternodeconfig.h \
  merkleblock.h \
  messagesigner.h \
  miner.h \
  mruset.h \
  netbase.h \
  net.h \
  noui.h \
  pow.h \
  protocol.h \
  pubkey.h \
  random.h \
  reverselock.h \
  reverse_iterate.h \
  rpc/client.h \
  rpc/protocol.h \
  rpc/server.h \
  scheduler.h \
  script/interpreter.h \
  script/script.h \
  script/sigcache.h \
  script/sign.h \
  script/standard.h \
  script/script_error.h \
  serialize.h \
  spork.h \
  sporkdb.h \
  sporkid.h \
  streams.h \
  support/cleanse.h \
  sync.h \
  threadsafety.h \
  timedata.h \
  tinyformat.h \
  torcontrol.h \
  txdb.h \
  txmempool.h \
  guiinterface.h \
  uint256.h \
  undo.h \
  util.h \
  utilstrencodings.h \
  utilmoneystr.h \
  utiltime.h \
  validationinterface.h \
  version.h \
  wallet/wallet.h \
  wallet/wallet_ismine.h \
  wallet/walletdb.h \
  concurrentqueue.h \
  zmq/zmqabstractnotifier.h \
  zmq/zmqconfig.h \
  zmq/zmqnotificationinterface.h \
  zmq/zmqpublishnotifier.h

obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh "$(abs_top_builddir)/src/obj/build.h" \
	  "$(abs_top_srcdir)"
libalqo_util_a-clientversion.$(OBJEXT): obj/build.h

# server: shared between alqod and alqo-qt
libalqo_server_a_CPPFLAGS = $(AM_CPPFLAGS) $(ALQO_INCLUDES) $(MINIUPNPC_CPPFLAGS) $(EVENT_CFLAGS) $(EVENT_PTHREADS_CFLAGS)
libalqo_server_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libalqo_server_a_SOURCES = \
  addrman.cpp \
  alert.cpp \
  bloom.cpp \
  blocksignature.cpp \
  chain.cpp \
  checkpoints.cpp \
  httprpc.cpp \
  httpserver.cpp \
  init.cpp \
  leveldbwrapper.cpp \
  main.cpp \
  merkleblock.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  pow.cpp \
  rest.cpp \
  rpc/blockchain.cpp \
  rpc/masternode.cpp \
  rpc/budget.cpp \
  rpc/mining.cpp \
  rpc/misc.cpp \
  rpc/net.cpp \
  rpc/rawtransaction.cpp \
  rpc/server.cpp \
  script/sigcache.cpp \
  sporkdb.cpp \
  timedata.cpp \
  torcontrol.cpp \
  txdb.cpp \
  txmempool.cpp \
  validationinterface.cpp \
  $(ALQO_CORE_H)

if ENABLE_ZMQ
libalqo_zmq_a_CPPFLAGS = $(ALQO_INCLUDES) $(ZMQ_CFLAGS)
libalqo_zmq_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libalqo_zmq_a_SOURCES = \
  zmq/zmqabstractnotifier.cpp \
  zmq/zmqnotificationinterface.cpp \
  zmq/zmqpublishnotifier.cpp
endif

# wallet: shared between alqod and alqo-qt, but only linked
# when wallet enabled
libalqo_wallet_a_CPPFLAGS = $(AM_CPPFLAGS) $(ALQO_INCLUDES)
libalqo_wallet_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libalqo_wallet_a_SOURCES = \
  activemasternode.cpp \
  bip38.cpp \
  obfuscation.cpp \
  obfuscation-relay.cpp \
  wallet/db.cpp \
  addressbook.cpp \
  crypter.cpp \
  swifttx.cpp \
  masternode.cpp \
  masternode-budget.cpp \
  masternode-payments.cpp \
  masternode-sync.cpp \
  masternodeconfig.cpp \
  masternodeman.cpp \
  messagesigner.cpp \
  wallet/rpcdump.cpp \
  wallet/rpcwallet.cpp \
  kernel.cpp \
  wallet/wallet.cpp \
  wallet/wallet_ismine.cpp \
  wallet/walletdb.cpp \
  $(ALQO_CORE_H)

# crypto primitives library
crypto_libalqo_crypto_a_CPPFLAGS = $(AM_CPPFLAGS) $(PIC_FLAGS)
crypto_libalqo_crypto_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIC_FLAGS)
crypto_libalqo_crypto_a_SOURCES = \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/chacha20.h \
  crypto/chacha20.cpp \
  crypto/hmac_sha256.cpp \
  crypto/rfc6979_hmac_sha256.cpp \
  crypto/hmac_sha512.cpp \
  crypto/scrypt.cpp \
  crypto/ripemd160.cpp \
  crypto/aes_helper.c \
  crypto/blake.c \
  crypto/bmw.c \
  crypto/groestl.c \
  crypto/jh.c \
  crypto/keccak.c \
  crypto/skein.c \
  crypto/common.h \
  crypto/sha256.h \
  crypto/sha512.h \
  crypto/hmac_sha256.h \
  crypto/rfc6979_hmac_sha256.h \
  crypto/hmac_sha512.h \
  crypto/scrypt.h \
  crypto/sha1.h \
  crypto/ripemd160.h \
  crypto/sph_blake.h \
  crypto/sph_bmw.h \
  crypto/sph_groestl.h \
  crypto/sph_jh.h \
  crypto/sph_keccak.h \
  crypto/sph_skein.h \
  crypto/sph_types.h

# common: shared between alqod, and alqo-qt and non-server tools
libalqo_common_a_CPPFLAGS = $(AM_CPPFLAGS) $(ALQO_INCLUDES)
libalqo_common_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libalqo_common_a_SOURCES = \
  allocators.cpp \
  amount.cpp \
  base58.cpp \
  bip38.cpp \
  chainparams.cpp \
  coins.cpp \
  compressor.cpp \
  primitives/block.cpp \
  primitives/transaction.cpp \
  core_read.cpp \
  core_write.cpp \
  hash.cpp \
  key.cpp \
  keystore.cpp \
  netbase.cpp \
  protocol.cpp \
  pubkey.cpp \
  scheduler.cpp \
  script/interpreter.cpp \
  script/script.cpp \
  script/sign.cpp \
  script/standard.cpp \
  script/script_error.cpp \
  spork.cpp \
  sporkdb.cpp \
  $(ALQO_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libalqo_util_a_CPPFLAGS = $(AM_CPPFLAGS) $(ALQO_INCLUDES)
libalqo_util_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libalqo_util_a_SOURCES = \
  allocators.cpp \
  chainparamsbase.cpp \
  clientversion.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  compat/strnlen.cpp \
  random.cpp \
  rpc/protocol.cpp \
  support/cleanse.cpp \
  sync.cpp \
  uint256.cpp \
  util.cpp \
  utilmoneystr.cpp \
  utilstrencodings.cpp \
  utiltime.cpp \
  $(ALQO_CORE_H)

if GLIBC_BACK_COMPAT
libalqo_util_a_SOURCES += compat/glibc_compat.cpp
AM_LDFLAGS += $(COMPAT_LDFLAGS)
endif

# cli: shared between alqo-cli and alqo-qt
libalqo_cli_a_CPPFLAGS = $(AM_CPPFLAGS) $(ALQO_INCLUDES)
libalqo_cli_a_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
libalqo_cli_a_SOURCES = \
  rpc/client.cpp \
  $(ALQO_CORE_H)

nodist_libalqo_util_a_SOURCES = $(srcdir)/obj/build.h
#

# alqod binary #
alqod_SOURCES = alqod.cpp
alqod_CPPFLAGS = $(AM_CPPFLAGS) $(ALQO_INCLUDES)
alqod_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
alqod_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
alqod_SOURCES += alqod-res.rc
endif
alqod_LDADD = \
  $(LIBALQO_SERVER) \
  $(LIBALQO_WALLET) \
  $(LIBALQO_COMMON) \
  $(LIBUNIVALUE) \
  $(LIBALQO_UTIL) \
  $(LIBALQO_ZMQ) \
  $(LIBALQO_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBLEVELDB_SSE42) \
  $(LIBMEMENV) \
  $(LIBSECP256K1)

alqod_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(MINIUPNPC_LIBS) $(EVENT_PTHREADS_LIBS) $(EVENT_LIBS) $(ZMQ_LIBS)

# alqo-cli binary #
alqo_cli_SOURCES = alqo-cli.cpp
alqo_cli_CPPFLAGS = $(AM_CPPFLAGS) $(ALQO_INCLUDES) $(EVENT_CFLAGS)
alqo_cli_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
alqo_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
alqo_cli_SOURCES += alqo-cli-res.rc
endif

alqo_cli_LDADD = \
  $(LIBALQO_CLI) \
  $(LIBUNIVALUE) \
  $(LIBALQO_UTIL) \
  $(LIBALQO_CRYPTO)

alqo_cli_LDADD += $(BOOST_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS) $(EVENT_LIBS)
#

# alqo-tx binary #
alqo_tx_SOURCES = alqo-tx.cpp
alqo_tx_CPPFLAGS = $(AM_CPPFLAGS) $(ALQO_INCLUDES)
alqo_tx_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)
alqo_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS) $(LIBTOOL_APP_LDFLAGS)

if TARGET_WINDOWS
alqo_tx_SOURCES += alqo-tx-res.rc
endif

alqo_tx_LDADD = \
  $(LIBUNIVALUE) \
  $(LIBALQO_COMMON) \
  $(LIBALQO_UTIL) \
  $(LIBALQO_CRYPTO) \
  $(LIBSECP256K1)

alqo_tx_LDADD += $(BOOST_LIBS) $(CRYPTO_LIBS)
#

# alqoconsensus library #
if BUILD_ALQO_LIBS
include_HEADERS = script/alqoconsensus.h
libalqoconsensus_la_SOURCES = \
  allocators.cpp \
  primitives/transaction.cpp \
  crypto/hmac_sha512.cpp \
  crypto/scrypt.cpp \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/ripemd160.cpp \
  hash.cpp \
  pubkey.cpp \
  script/script.cpp \
  script/interpreter.cpp \
  script/alqoconsensus.cpp \
  uint256.cpp \
  utilstrencodings.cpp

if GLIBC_BACK_COMPAT
  libalqoconsensus_la_SOURCES += compat/glibc_compat.cpp
endif

libalqoconsensus_la_LDFLAGS = $(AM_LDFLAGS) -no-undefined $(RELDFLAGS)
libalqoconsensus_la_LIBADD = $(LIBSECP256K1)
libalqoconsensus_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(builddir)/obj -I$(srcdir)/secp256k1/include -DBUILD_ALQO_INTERNAL
libalqoconsensus_la_CXXFLAGS = $(AM_CXXFLAGS) $(PIE_FLAGS)

endif
#

CLEANFILES = $(EXTRA_LIBRARIES)

CLEANFILES += *.gcda *.gcno
CLEANFILES += compat/*.gcda compat/*.gcno
CLEANFILES += crypto/*.gcda crypto/*.gcno
CLEANFILES += primitives/*.gcda primitives/*.gcno
CLEANFILES += rpc/*.gcda rpc/*.gcno
CLEANFILES += script/*.gcda script/*.gcno
CLEANFILES += support/*.gcda support/*.gcno
CLEANFILES += univalue/*.gcda univalue/*.gcno
CLEANFILES += wallet/*.gcda wallet/*.gcno
CLEANFILES += wallet/test/*.gcda wallet/test/*.gcno
CLEANFILES += zmq/*.gcda zmq/*.gcno
CLEANFILES += obj/build.h

EXTRA_DIST =


config/alqo-config.h: config/stamp-h1
	@$(MAKE) -C $(top_builddir) $(subdir)/$(@)
config/stamp-h1: $(top_srcdir)/$(subdir)/config/alqo-config.h.in $(top_builddir)/config.status
	$(AM_V_at)$(MAKE) -C $(top_builddir) $(subdir)/$(@)
$(top_srcdir)/$(subdir)/config/alqo-config.h.in:  $(am__configure_deps)
	$(AM_V_at)$(MAKE) -C $(top_srcdir) $(subdir)/config/alqo-config.h.in

clean-local:
	-$(MAKE) -C secp256k1 clean
	-$(MAKE) -C univalue clean
	-rm -f leveldb/*/*.gcda leveldb/*/*.gcno leveldb/helpers/memenv/*.gcda leveldb/helpers/memenv/*.gcno
	-rm -f config.h
	-rm -rf test/__pycache__

.rc.o:
	@test -f $(WINDRES)
	## FIXME: How to get the appropriate modulename_CPPFLAGS in here?
	$(AM_V_GEN) $(WINDRES) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) -DWINDRES_PREPROC -i $< -o $@

check-symbols: $(bin_PROGRAMS)
if GLIBC_BACK_COMPAT
	@echo "Checking glibc back compat..."
	$(AM_V_at) READELF=$(READELF) CPPFILT=$(CPPFILT) $(PYTHON) $(top_srcdir)/contrib/devtools/symbol-check.py < $(bin_PROGRAMS)
endif

check-security: $(bin_PROGRAMS)
if HARDEN
	@echo "Checking binary security..."
	$(AM_V_at) READELF=$(READELF) OBJDUMP=$(OBJDUMP) $(PYTHON) $(top_srcdir)/contrib/devtools/security-check.py < $(bin_PROGRAMS)
endif

%.pb.cc %.pb.h: %.proto
	@test -f $(PROTOC)
	$(AM_V_GEN) $(PROTOC) --cpp_out=$(@D) --proto_path=$(<D) $<

if EMBEDDED_LEVELDB
include Makefile.leveldb.include
endif

if ENABLE_QT
include Makefile.qt.include
endif

