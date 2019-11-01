/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright(c) 2019 Intel Corporation
 */
#ifndef __GTP_COMMON_H__
#define __GTP_COMMON_H__

#include <cstdint>
#include <time.h>
/*----------------------------------------------------------------------------------*/
#define DPN_ID 12345
/**
 * Maximum buffer/name length
 */
#define MAX_LEN 128

/**
 * MAX DNS Sponsor ID name lenth
 */
#define MAX_DNS_SPON_ID_LEN 16

/**
 * IPv6 address length
 */
#define IPV6_ADDR_LEN 16

/**
 * Maximum PCC rules per session.
 */
#define MAX_PCC_RULES 12

/**
 * Maximum PCC rules per session.
 */
#define MAX_ADC_RULES 16

/**
 * Maximum CDR services.
 */
#define MAX_SERVICE 1

enum { MSG_SESS_CRE = 2, MSG_SESS_MOD, MSG_SESS_DEL };

enum {
  DPN_RESPONSE = 4,
  DPN_CREATE_RESP = 10,
  DPN_MODIFY_RESP,
  DPN_DELETE_RESP
};

/**
 *  Select IPv4 or IPv6.
 */
enum iptype {
  IPTYPE_IPV4 = 0, /* IPv4. */
  IPTYPE_IPV6,     /* IPv6. */
};

/**
 * check if nth bit is set.
 */
#define ISSET_BIT(mask, n) (((mask) & (1LLU << (n))) ? 1 : 0)

/**
 * Default bearer session
 */
#define DEFAULT_BEARER 5

/**
 * set session id from the combination of
 * unique UE addr and Bearer id
 */
#define SESS_ID(ue_addr, br_id) \
  (((uint64_t)(br_id) << 32) | (0xffffffff & (ue_addr)))
/* [0] 28 bits | [bearer-id] 4 bits | [ue-addr] 32 bits */
/**
 * get bearer id
 */
#define UE_BEAR_ID(x) (x >> 32)

/**
 * get ue_addr
 */
#define UE_ADDR(x) (x & 0xffffffff)
/*----------------------------------------------------------------------------------*/
/**
 *  DP cdr_msg resp_msgbuf structure
 */
struct cdr_msg {
	uint32_t ratingGroup;
	uint32_t chargingID;
	uint64_t localSequenceNumber;
	uint64_t recordOpeningTime;
	uint64_t timeOfFirstUsage;
	uint64_t timeOfLastUsage;
	uint64_t timeUsage;
	uint8_t record_closure_cause;
	uint64_t datavolumeFBCUplink;
	uint64_t datavolumeFBCDownlink;
	uint64_t timeOfReport;
	void *ue_context;
	void *session_info;
	uint32_t apn_idx;
};

/**
 * IPv4 or IPv6 address configuration structure.
 */
struct ip_addr {
  enum iptype iptype; /* IP type: IPv4 or IPv6. */
  union {
    uint32_t ipv4_addr;               /* IPv4 address*/
    uint8_t ipv6_addr[IPV6_ADDR_LEN]; /* IPv6 address*/
  } u;
} __attribute__((packed, aligned(64)));

/**
 * UpLink S1u interface config structure.
 */
struct ul_s1_info {
  uint32_t sgw_teid;             /* SGW teid */
  struct ip_addr enb_addr;       /* eNodeB address */
  struct ip_addr sgw_addr;       /* Serving Gateway address */
  struct ip_addr s5s8_pgwu_addr; /* S5S8_PGWU address */
} __attribute__((packed, aligned(64)));

/**
 * DownLink S1u interface config structure.
 */
struct dl_s1_info {
  uint32_t enb_teid;             /* eNodeB teid */
  struct ip_addr enb_addr;       /* eNodeB address */
  struct ip_addr sgw_addr;       /* Serving Gateway address */
  struct ip_addr s5s8_sgwu_addr; /* S5S8_SGWU address */
} __attribute__((packed, aligned(64)));

/**
 * Packet filter configuration structure.
 */
struct service_data_list {
  uint32_t service[MAX_SERVICE]; /* list of service id*/
                                 /* TODO: add other members*/
};

struct cdr {
  uint64_t bytes;
  uint64_t pkt_count;
};

/**
 * Volume based Charging
 */
struct chrg_data_vol {
	struct cdr ul_cdr;              /* Uplink cdr */
	struct cdr dl_cdr;              /* Downlink cdr */
	struct cdr ul_cdr_last;		/* last sent uplink cdr */
	struct cdr dl_cdr_last;		/* last sent downlink cdr */
	uint64_t ul_bytes_delta;	/* bytes b/w previous record and current */
	uint64_t dl_bytes_delta;	/* bytes b/w previous record and current */
	struct cdr ul_drop;             /* Uplink dropped cdr */
	struct cdr dl_drop;             /* Downlink dropped cdr */
};

/**
 * IP-CAN Bearer Charging Data Records
 */
struct ipcan_dp_bearer_cdr {
	uint32_t charging_id;				/* Bearer Charging id*/
	uint32_t pdn_conn_charging_id;			/* PDN connection charging id*/
	struct tm record_open_time;			/* Record time*/
	uint64_t time_of_first_use;			/* Time of first use */
	uint64_t duration_time;				/* duration (sec)*/
	uint8_t	record_closure_cause;			/* Record closure cause*/
	uint64_t record_seq_number;			/* Sequence no.*/
	uint8_t charging_behavior_index; 		/* Charging index*/
	uint32_t service_id;				/* to identify the service
						 	 * or the service component
						 	 * the bearer relates to*/
	char sponsor_id[MAX_DNS_SPON_ID_LEN];	        /* to identify the 3rd party organization (the
						 	 * sponsor) willing to pay for the operator's charge*/
	struct service_data_list service_data_list; 	/* List of service*/
	uint32_t rating_group;				/* rating group of this bearer*/
	uint64_t tmr_trshld;				/* timer threshold in seconds */
	uint64_t vol_threshold;				/* volume threshold in MBytes*/
	struct chrg_data_vol data_vol;			/* charing per UE by volume*/
	uint32_t charging_rule_id;			/* Charging Rule ID*/
} __attribute__((packed, aligned(64)));

/**
 * Bearer Session information structure
 */
#include <netinet/in.h>

#define MAX_BEARERS                  (11)
#define MAX_FILTERS_PER_UE           (16)
/**
 * IE specific data segment for Quality of Service (QoS).
 *
 * Definition used by bearer_qos_ie and flow_qos_ie.
 */
typedef struct qos_segment_t {
	/** QoS class identifier - defined by 3GPP TS 23.203 */
	uint8_t qci;

	/** Uplink Maximum Bit Rate in kilobits (1000bps) - for non-GBR
         * Bearers this field to be set to zero*/
	uint64_t ul_mbr;
	/** Downlink Maximum Bit Rate in kilobits (1000bps) - for non-GBR
         * Bearers this field to be set to zero*/
	uint64_t dl_mbr;
	/** Uplink Guaranteed Bit Rate in kilobits (1000bps) - for non-GBR
         * Bearers this field to be set to zero*/
	uint64_t ul_gbr;
	/** Downlink Guaranteed Bit Rate in kilobits (1000bps) - for non-GBR
         * Bearers this field to be set to zero*/
	uint64_t dl_gbr;
} qos_segment;

/**
 * IE specific data for Bearer Quality of Service (QoS) as defined by
 * 3GPP TS 29.274, clause 8.15 for the IE type value 80.
 */
typedef struct ar_priority_ie_t {
	uint8_t preemption_vulnerability :1;
	uint8_t spare1 :1;
	uint8_t priority_level :4;
	uint8_t preemption_capability :1;
	uint8_t spare2 :1;
} ar_priority_ie;

/**
 * IE specific data for Bearer Quality of Service (QoS) as defined by
 * 3GPP TS 29.274, clause 8.15 for the IE type value 80.
 * TODO: REMOVE_DUPLICATE_USE_LIBGTPV2C
 * Remove following structure and use structure defined in
 * libgtpv2c header file.
 * Following structure has dependency on functionality
 * which can not to be tested now.
 */
typedef struct bearer_qos_ie {
	/* First Byte: Allocation/Retention Priority (ARP) */
	ar_priority_ie arp;
	qos_segment qos;
} bearer_qos_ie;

struct pdn_connection_t;

typedef struct eps_bearer_t {
	uint8_t eps_bearer_id;

	bearer_qos_ie qos;

	uint32_t charging_id;

	struct in_addr s1u_sgw_gtpu_ipv4;
	uint32_t s1u_sgw_gtpu_teid;
	struct in_addr s5s8_sgw_gtpu_ipv4;
	uint32_t s5s8_sgw_gtpu_teid;
	struct in_addr s5s8_pgw_gtpu_ipv4;
	uint32_t s5s8_pgw_gtpu_teid;
	struct in_addr s1u_enb_gtpu_ipv4;
	uint32_t s1u_enb_gtpu_teid;

	struct in_addr s11u_mme_gtpu_ipv4;
	uint32_t s11u_mme_gtpu_teid;

	struct pdn_connection_t *pdn;

	int packet_filter_map[MAX_FILTERS_PER_UE];
	uint8_t num_packet_filters;
} eps_bearer;

/**
 * IE specific data for Charging Characteristics as defined by
 * 3GPP TS 29.274, clause 8.30 for the IE type value 95.
 *
 * Charging characteristics information element is defined in 3GPP TS 32.251
 *
 * For the encoding of this information element see 3GPP TS 32.298
 */
typedef struct charging_characteristics_ie_t {
	uint8_t b0 :1;
	uint8_t b1 :1;
	uint8_t b2 :1;
	uint8_t b3 :1;
	uint8_t b4 :1;
	uint8_t b5 :1;
	uint8_t b6 :1;
	uint8_t b7 :1;
	uint8_t b8 :1;
	uint8_t b9 :1;
	uint8_t b10 :1;
	uint8_t b11 :1;
	uint8_t b12 :1;
	uint8_t b13 :1;
	uint8_t b14 :1;
	uint8_t b15 :1;
} charging_characteristics_ie;

/**
 * IE specific data for Packet Data Network (PDN) Type as defined by
 * 3GPP TS 29.274, clause 8.34 for the IE type value 99.
 * TODO: REMOVE_DUPLICATE_USE_LIBGTPV2C
 * Remove following structure and use structure defined in
 * libgtpv2c header file.
 * Following structure has dependency on functionality
 * which can not to be tested now.
 */
typedef struct pdn_type_ie {
	uint8_t ipv4 :1;
	uint8_t ipv6 :1;
	uint8_t spare :6;
} pdn_type_ie;

/**
 * IE specific data for Aggregate Maximum Bit Rate (AMBR) as defined by
 * 3GPP TS 29.274, clause 8.7 for the IE type value 72.
 * TODO: REMOVE_DUPLICATE_USE_LIBGTPV2C
 * Remove following structure and use structure defined in
 * libgtpv2c header file.
 * Following structure has dependency on functionality
 * which can not to be tested now.
 */
typedef struct ambr_ie {
	uint32_t ambr_uplink;
	uint32_t ambr_downlink;
} ambr_ie;

typedef struct apn_t {
	char *apn_name_label;
	size_t apn_name_length;
	uint8_t apn_idx;
} apn;

typedef struct pdn_connection_t {
	apn *apn_in_use;
	ambr_ie apn_ambr;
	uint32_t apn_restriction;

	ambr_ie session_ambr;
	ambr_ie session_gbr;

	struct in_addr ipv4;
	struct in6_addr ipv6;

	uint32_t s5s8_sgw_gtpc_teid;
	struct in_addr s5s8_sgw_gtpc_ipv4;

	uint32_t s5s8_pgw_gtpc_teid;
	struct in_addr s5s8_pgw_gtpc_ipv4;

	pdn_type_ie pdn_type;

	/* LINK::charging_characteristics_ie */
	charging_characteristics_ie charging_characteristics;

	uint8_t default_bearer_id;

	struct eps_bearer_t *eps_bearers[MAX_BEARERS]; /* index by ebi - 5 */

	struct eps_bearer_t *packet_filter_map[MAX_FILTERS_PER_UE];
} pdn_connection;

struct session_info {
	void *dp_session;						/* session_info: CP CDR collation handle */
	void *ue_context;						/* ue_context: CP CDR collation handle */
	struct pdn_connection_t *pdn_context;				/* pdn_connection: CP CDR collation handle */

	uint8_t apn_idx;						/* apn reference to DP */
	struct ip_addr ue_addr;						/* UE ip address */
	struct ul_s1_info ul_s1_info;					/* Uplink S1u info */
	struct dl_s1_info dl_s1_info;					/* Downlink s1u info */
	uint8_t bearer_id;						/* Bearer ID */

	/* PCC rules reltaed params */
	uint32_t num_ul_pcc_rules;					/* No. of UL PCC rule */
	uint32_t ul_pcc_rule_id[MAX_PCC_RULES];				/* PCC rule id supported in UL */
	uint32_t num_dl_pcc_rules;					/* No. of PCC rule */
	uint32_t dl_pcc_rule_id[MAX_PCC_RULES];				/* PCC rule id supported in UL */

	/* ADC rules related params */
	uint32_t num_adc_rules;						/* No. of ADC rule */
	uint32_t adc_rule_id[MAX_ADC_RULES];				/* List of ADC rule id */

	/* Charging Data Records */
	struct ipcan_dp_bearer_cdr ipcan_dp_bearer_cdr;			/* Charging Data Records */
	uint32_t client_id;

	uint64_t op_id;
	uint64_t sess_id;						/** session id of this bearer
									 * last 4 bits of sess_id
									 * maps to bearer id
									 */
	uint32_t service_id;						/** Type of service given
									 * given to this session like
									 * Internet, Management, CIPA etc
									 */
	uint32_t ul_apn_mtr_idx;					/* UL APN meter profile index */
	uint32_t dl_apn_mtr_idx;	       				/* DL APN meter profile index */
} __attribute__((packed, aligned(64)));

/**
 * DataPlane identifier information structure.
 */
struct dp_id {
  uint64_t id;        /* table identifier.*/
  char name[MAX_LEN]; /* name string of identifier*/
} __attribute__((packed, aligned(64)));

/*
 * Response Message Structure
 */
struct resp_msgbuf {
	long mtype;
	uint64_t op_id;
	uint64_t sess_id;
	struct dp_id dp_id;
	struct cdr_msg cdr_msg;
};

/*
 * Message Structure
 */
struct msgbuf {
  long mtype;
  struct dp_id dp_id;
  struct session_info sess_entry;
};
/*----------------------------------------------------------------------------------*/
#endif /* !__GTP_COMMON_H__ */
