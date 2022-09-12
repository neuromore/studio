
include platforms/detect-host.mk

NAME       = libcrypto
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DHAVE_CONFIG_H \
             -DOPENSSL_STATIC \
             -DOPENSSL_NO_DYNAMIC_ENGINE \
             -DOPENSSL_THREADS \
             -DOPENSSL_PIC \
             -DOPENSSL_USE_APPLINK \
             -DL_ENDIAN
INCLUDES  := $(INCLUDES) \
             -I$(INCDIR) \
             -I$(INCDIR)/openssl \
             -I$(SRCDIR)/modes \
             -I$(SRCDIR)/../openssl
CFLAGS    := $(CFLAGS) \
             -Wno-unknown-warning-option \
             -Wno-implicit-int \
             -Wno-format \
             -Wno-incompatible-pointer-types \
             -Wno-constant-conversion \
             -Wno-ignored-pragma-optimize
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = aes/aes_cbc.o aes/aes_cfb.o aes/aes_ecb.o aes/aes_ige.o aes/aes_misc.o aes/aes_ofb.o \
             aes/aes_wrap.o aes/aes_x86core.o asn1/a_bitstr.o asn1/a_d2i_fp.o asn1/a_digest.o \
             asn1/a_dup.o asn1/a_gentm.o asn1/a_i2d_fp.o asn1/a_int.o asn1/a_mbstr.o asn1/a_object.o \
             asn1/a_octet.o asn1/a_print.o asn1/a_sign.o asn1/a_strex.o asn1/a_strnid.o asn1/a_time.o \
             asn1/a_type.o asn1/a_utctm.o asn1/a_utf8.o asn1/a_verify.o asn1/ameth_lib.o asn1/asn_mime.o \
             asn1/asn_moid.o asn1/asn_mstbl.o asn1/asn_pack.o asn1/asn1_err.o asn1/asn1_gen.o \
             asn1/asn1_lib.o asn1/asn1_par.o asn1/bio_asn1.o asn1/bio_ndef.o asn1/d2i_pr.o asn1/d2i_pu.o \
             asn1/evp_asn1.o asn1/f_int.o asn1/f_string.o asn1/i2d_pr.o asn1/i2d_pu.o \
             asn1/n_pkey.o asn1/nsseq.o asn1/p5_pbe.o asn1/p5_pbev2.o asn1/p5_scrypt.o asn1/p8_pkey.o \
             asn1/t_bitst.o asn1/t_pkey.o asn1/t_spki.o asn1/tasn_dec.o asn1/tasn_enc.o asn1/tasn_fre.o \
             asn1/tasn_new.o asn1/tasn_prn.o asn1/tasn_scn.o asn1/tasn_typ.o asn1/tasn_utl.o \
             asn1/x_algor.o asn1/x_bignum.o asn1/x_info.o asn1/x_int64.o asn1/x_long.o asn1/x_pkey.o \
             asn1/x_sig.o asn1/x_spki.o asn1/x_val.o async/arch/async_null.o async/arch/async_posix.o \
             async/arch/async_win.o async/async.o async/async_err.o async/async_wait.o \
             bf/bf_cfb64.o bf/bf_ecb.o bf/bf_enc.o bf/bf_ofb64.o bf/bf_skey.o bio/b_addr.o bio/b_dump.o \
             bio/b_print.o bio/b_sock.o bio/b_sock2.o bio/bf_buff.o bio/bf_lbuf.o bio/bf_nbio.o \
             bio/bf_null.o bio/bio_cb.o bio/bio_err.o bio/bio_lib.o bio/bio_meth.o bio/bss_acpt.o \
             bio/bss_bio.o bio/bss_conn.o bio/bss_dgram.o bio/bss_fd.o bio/bss_file.o bio/bss_log.o \
             bio/bss_mem.o bio/bss_null.o bio/bss_sock.o blake2/blake2b.o blake2/blake2s.o \
             blake2/m_blake2b.o blake2/m_blake2s.o bn/bn_add.o bn/bn_asm.o bn/bn_blind.o bn/bn_const.o \
             bn/bn_ctx.o bn/bn_depr.o bn/bn_dh.o bn/bn_div.o bn/bn_err.o bn/bn_exp.o bn/bn_exp2.o \
             bn/bn_gcd.o bn/bn_gf2m.o bn/bn_intern.o bn/bn_kron.o bn/bn_lib.o bn/bn_mod.o bn/bn_mont.o \
             bn/bn_mpi.o bn/bn_mul.o bn/bn_nist.o bn/bn_prime.o bn/bn_print.o bn/bn_rand.o bn/bn_recp.o \
             bn/bn_shift.o bn/bn_sqr.o bn/bn_sqrt.o bn/bn_srp.o bn/bn_word.o bn/bn_x931p.o buffer/buf_err.o \
             buffer/buffer.o camellia/camellia.o camellia/cmll_cbc.o camellia/cmll_cfb.o camellia/cmll_ctr.o \
             camellia/cmll_ecb.o camellia/cmll_misc.o camellia/cmll_ofb.o cast/c_cfb64.o cast/c_ecb.o \
             cast/c_enc.o cast/c_ofb64.o cast/c_skey.o chacha/chacha_enc.o cmac/cm_ameth.o cmac/cm_pmeth.o \
             cmac/cmac.o cms/cms_asn1.o cms/cms_att.o cms/cms_cd.o cms/cms_dd.o cms/cms_enc.o cms/cms_env.o \
             cms/cms_err.o cms/cms_ess.o cms/cms_io.o cms/cms_kari.o cms/cms_lib.o cms/cms_pwri.o \
             cms/cms_sd.o cms/cms_smime.o comp/c_zlib.o comp/comp_err.o comp/comp_lib.o conf/conf_api.o \
             conf/conf_def.o conf/conf_err.o conf/conf_lib.o conf/conf_mall.o conf/conf_mod.o conf/conf_sap.o \
             conf/conf_ssl.o ct/ct_b64.o ct/ct_err.o ct/ct_log.o ct/ct_oct.o ct/ct_policy.o ct/ct_prn.o \
             ct/ct_sct.o ct/ct_sct_ctx.o ct/ct_vfy.o ct/ct_x509v3.o des/cbc_cksm.o des/cbc_enc.o \
             des/cfb_enc.o des/cfb64ede.o des/cfb64enc.o des/des_enc.o des/ecb_enc.o des/ecb3_enc.o \
             des/fcrypt.o des/fcrypt_b.o des/ofb_enc.o des/ofb64ede.o des/ofb64enc.o des/pcbc_enc.o \
             des/qud_cksm.o des/rand_key.o des/rpc_enc.o des/set_key.o des/str2key.o des/xcbc_enc.o \
             dh/dh_ameth.o dh/dh_asn1.o dh/dh_check.o dh/dh_depr.o dh/dh_err.o dh/dh_gen.o dh/dh_kdf.o \
             dh/dh_key.o dh/dh_lib.o dh/dh_meth.o dh/dh_pmeth.o dh/dh_prn.o dh/dh_rfc5114.o dsa/dsa_ameth.o \
             dsa/dsa_asn1.o dsa/dsa_depr.o dsa/dsa_err.o dsa/dsa_gen.o dsa/dsa_key.o dsa/dsa_lib.o \
             dsa/dsa_meth.o dsa/dsa_ossl.o dsa/dsa_pmeth.o dsa/dsa_prn.o dsa/dsa_sign.o dsa/dsa_vrf.o \
             dso/dso_dl.o dso/dso_dlfcn.o dso/dso_err.o dso/dso_lib.o dso/dso_openssl.o dso/dso_vms.o \
             dso/dso_win32.o ec/curve25519.o ec/ec_ameth.o ec/ec_asn1.o ec/ec_check.o ec/ec_curve.o \
             ec/ec_cvt.o ec/ec_err.o ec/ec_key.o ec/ec_kmeth.o ec/ec_lib.o ec/ec_mult.o ec/ec_oct.o \
             ec/ec_pmeth.o ec/ec_print.o ec/ec2_mult.o ec/ec2_oct.o ec/ec2_smpl.o ec/ecdh_kdf.o ec/ecdh_ossl.o \
             ec/ecdsa_ossl.o ec/ecdsa_sign.o ec/ecdsa_vrf.o ec/eck_prn.o ec/ecp_mont.o ec/ecp_nist.o \
             ec/ecp_nistz256.o ec/ecp_oct.o ec/ecp_smpl.o ec/ecx_meth.o engine/eng_all.o engine/eng_cnf.o \
             engine/eng_cryptodev.o engine/eng_ctrl.o engine/eng_dyn.o engine/eng_err.o engine/eng_fat.o \
             engine/eng_init.o engine/eng_lib.o engine/eng_list.o engine/eng_openssl.o engine/eng_pkey.o \
             engine/eng_rdrand.o engine/eng_table.o engine/tb_asnmth.o engine/tb_cipher.o engine/tb_dh.o \
             engine/tb_digest.o engine/tb_dsa.o engine/tb_eckey.o engine/tb_pkmeth.o engine/tb_rand.o \
             engine/tb_rsa.o err/err.o err/err_all.o err/err_prn.o evp/bio_b64.o evp/bio_enc.o evp/bio_md.o \
             evp/bio_ok.o evp/c_allc.o evp/c_alld.o evp/cmeth_lib.o evp/digest.o evp/e_aes.o \
             evp/e_aes_cbc_hmac_sha1.o evp/e_aes_cbc_hmac_sha256.o evp/e_bf.o evp/e_camellia.o evp/e_cast.o \
             evp/e_chacha20_poly1305.o evp/e_des.o evp/e_des3.o evp/e_idea.o evp/e_null.o evp/e_old.o \
             evp/e_rc2.o evp/e_rc4.o evp/e_rc4_hmac_md5.o evp/e_rc5.o evp/e_seed.o evp/e_xcbc_d.o evp/encode.o \
             evp/evp_cnf.o evp/evp_enc.o evp/evp_err.o evp/evp_key.o evp/evp_lib.o evp/evp_pbe.o evp/evp_pkey.o \
             evp/m_md2.o evp/m_md4.o evp/m_md5.o evp/m_md5_sha1.o evp/m_mdc2.o evp/m_null.o evp/m_ripemd.o \
             evp/m_sha1.o evp/m_sigver.o evp/m_wp.o evp/names.o evp/p_dec.o evp/p_enc.o evp/p_lib.o evp/p_open.o \
             evp/p_seal.o evp/p_sign.o evp/p_verify.o evp/p5_crpt.o evp/p5_crpt2.o evp/pmeth_fn.o evp/pmeth_gn.o \
             evp/pmeth_lib.o evp/scrypt.o hmac/hm_ameth.o hmac/hm_pmeth.o hmac/hmac.o idea/i_cbc.o \
             idea/i_cfb64.o idea/i_ecb.o idea/i_ofb64.o idea/i_skey.o kdf/hkdf.o kdf/kdf_err.o kdf/tls1_prf.o \
             lhash/lh_stats.o lhash/lhash.o md2/md2_dgst.o md2/md2_one.o md4/md4_dgst.o md4/md4_one.o \
             md5/md5_dgst.o md5/md5_one.o mdc2/mdc2_one.o mdc2/mdc2dgst.o modes/cbc128.o modes/ccm128.o \
             modes/cfb128.o modes/ctr128.o modes/cts128.o modes/gcm128.o modes/ocb128.o modes/ofb128.o \
             modes/wrap128.o modes/xts128.o objects/o_names.o objects/obj_dat.o objects/obj_err.o \
             objects/obj_lib.o objects/obj_xref.o ocsp/ocsp_asn.o ocsp/ocsp_cl.o ocsp/ocsp_err.o ocsp/ocsp_ext.o \
             ocsp/ocsp_ht.o ocsp/ocsp_lib.o ocsp/ocsp_prn.o ocsp/ocsp_srv.o ocsp/ocsp_vfy.o ocsp/v3_ocsp.o \
             pem/pem_all.o pem/pem_err.o pem/pem_info.o pem/pem_lib.o pem/pem_oth.o pem/pem_pk8.o pem/pem_pkey.o \
             pem/pem_sign.o pem/pem_x509.o pem/pem_xaux.o pem/pvkfmt.o pkcs7/bio_pk7.o pkcs7/pk7_asn1.o \
             pkcs7/pk7_attr.o pkcs7/pk7_dgst.o pkcs7/pk7_doit.o pkcs7/pk7_lib.o pkcs7/pk7_mime.o \
             pkcs7/pk7_smime.o pkcs7/pkcs7err.o pkcs12/p12_add.o pkcs12/p12_asn.o pkcs12/p12_attr.o \
             pkcs12/p12_crpt.o pkcs12/p12_crt.o pkcs12/p12_decr.o pkcs12/p12_init.o pkcs12/p12_key.o \
             pkcs12/p12_kiss.o pkcs12/p12_mutl.o pkcs12/p12_npas.o pkcs12/p12_p8d.o pkcs12/p12_p8e.o \
             pkcs12/p12_sbag.o pkcs12/p12_utl.o pkcs12/pk12err.o poly1305/poly1305.o rand/md_rand.o \
             rand/rand_egd.o rand/rand_err.o rand/rand_lib.o rand/rand_unix.o rand/rand_win.o \
             rand/randfile.o rc2/rc2_cbc.o rc2/rc2_ecb.o rc2/rc2_skey.o rc2/rc2cfb64.o rc2/rc2ofb64.o \
             rc2/tab.o rc4/rc4_enc.o rc4/rc4_skey.o rc5/rc5_ecb.o rc5/rc5_enc.o rc5/rc5_skey.o rc5/rc5cfb64.o \
             rc5/rc5ofb64.o ripemd/rmd_dgst.o ripemd/rmd_one.o rsa/rsa_ameth.o rsa/rsa_asn1.o rsa/rsa_chk.o \
             rsa/rsa_crpt.o rsa/rsa_depr.o rsa/rsa_err.o rsa/rsa_gen.o rsa/rsa_lib.o rsa/rsa_meth.o \
             rsa/rsa_none.o rsa/rsa_null.o rsa/rsa_oaep.o rsa/rsa_ossl.o rsa/rsa_pk1.o rsa/rsa_pmeth.o \
             rsa/rsa_prn.o rsa/rsa_pss.o rsa/rsa_saos.o rsa/rsa_sign.o rsa/rsa_ssl.o rsa/rsa_x931.o \
             rsa/rsa_x931g.o seed/seed.o seed/seed_cbc.o seed/seed_cfb.o seed/seed_ecb.o seed/seed_ofb.o \
             sha/sha1_one.o sha/sha1dgst.o sha/sha256.o sha/sha512.o srp/srp_lib.o srp/srp_vfy.o \
             stack/stack.o ts/ts_asn1.o ts/ts_conf.o ts/ts_err.o ts/ts_lib.o ts/ts_req_print.o ts/ts_req_utils.o \
             ts/ts_rsp_print.o ts/ts_rsp_sign.o ts/ts_rsp_utils.o ts/ts_rsp_verify.o ts/ts_verify_ctx.o \
             txt_db/txt_db.o ui/ui_err.o ui/ui_lib.o ui/ui_openssl.o ui/ui_util.o whrlpool/wp_block.o \
             whrlpool/wp_dgst.o x509/by_dir.o x509/by_file.o x509/t_crl.o x509/t_req.o x509/t_x509.o x509/x_all.o \
             x509/x_attrib.o x509/x_crl.o x509/x_exten.o x509/x_name.o x509/x_pubkey.o x509/x_req.o x509/x_x509.o \
             x509/x_x509a.o x509/x509_att.o x509/x509_cmp.o x509/x509_d2.o x509/x509_def.o x509/x509_err.o \
             x509/x509_ext.o x509/x509_lu.o x509/x509_meth.o x509/x509_obj.o x509/x509_r2x.o x509/x509_req.o \
             x509/x509_set.o x509/x509_trs.o x509/x509_txt.o x509/x509_v3.o x509/x509_vfy.o x509/x509_vpm.o \
             x509/x509cset.o x509/x509name.o x509/x509rset.o x509/x509spki.o x509/x509type.o x509v3/pcy_cache.o \
             x509v3/pcy_data.o x509v3/pcy_lib.o x509v3/pcy_map.o x509v3/pcy_node.o x509v3/pcy_tree.o \
             x509v3/v3_addr.o x509v3/v3_akey.o x509v3/v3_akeya.o x509v3/v3_alt.o x509v3/v3_asid.o \
             x509v3/v3_bcons.o x509v3/v3_bitst.o x509v3/v3_conf.o x509v3/v3_cpols.o x509v3/v3_crld.o \
             x509v3/v3_enum.o x509v3/v3_extku.o x509v3/v3_genn.o x509v3/v3_ia5.o x509v3/v3_info.o \
             x509v3/v3_int.o x509v3/v3_lib.o x509v3/v3_ncons.o x509v3/v3_pci.o x509v3/v3_pcia.o x509v3/v3_pcons.o \
             x509v3/v3_pku.o x509v3/v3_pmaps.o x509v3/v3_prn.o x509v3/v3_purp.o x509v3/v3_skey.o x509v3/v3_sxnet.o \
             x509v3/v3_tlsf.o x509v3/v3_utl.o x509v3/v3err.o cpt_err.o cryptlib.o cversion.o dllmain.o ex_data.o \
             getenv.o init.o mem.o mem_clr.o mem_dbg.o mem_sec.o o_dir.o o_fips.o o_fopen.o o_init.o o_str.o \
             o_time.o threads_none.o threads_pthread.o threads_win.o uid.o

ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DOPENSSL_IA32_SSE2
endif

ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DOPENSSL_IA32_SSE2
endif

ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES) \
             -D_WINSOCK_DEPRECATED_NO_WARNINGS \
             -D_CRT_SECURE_NO_WARNINGS \
             -DDSO_WIN32 \
             -DWIN32_LEAN_AND_MEAN \
             -DOPENSSLDIR=\".\" \
             -DENGINESDIR=\".\" 
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DOPENSSL_SYS_WIN32
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DOPENSSL_SYS_WIN64
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES) \
             -DOPENSSLDIR=\"/etc/ssl\" \
             -DENGINESDIR=\".\"
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES) \
             -DOPENSSLDIR=\"/etc/ssl\" \
             -DENGINESDIR=\".\"
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES) \
             -DOPENSSLDIR=\"/etc/ssl\" \
             -DENGINESDIR=\".\" 
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

OBJS := $(patsubst %,$(OBJDIR)/%,$(OBJS))

$(OBJDIR)/%.o:
	@echo [CC]  $@
	$(CC) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.c) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
