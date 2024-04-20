\c icc;

-----------------------------------------------------------接警单表分表初始化--------------------------------------------------------------
select init_shard_icc_t_jjdb();

-----------------------------------------------------------派警单表分表初始化--------------------------------------------------------------
select init_shard_icc_t_pjdb();

-----------------------------------------------------------流水表分表初始化--------------------------------------------------------------
select init_shard_icc_t_alarm_log();

-----------------------------------------------------------话务表分表初始化--------------------------------------------------------------
select init_shard_icc_t_callevent();

-----------------------------------------------------------话务子表分表初始化--------------------------------------------------------------
select init_shard_icc_t_sub_callevent();

-----------------------------------------------------------反馈单表分表初始化--------------------------------------------------------------
select init_shard_icc_t_fkdb();

-----------------------------------------------------------录音文件记录表初始化----------------------------------------------------------------
select init_shard_icc_t_record_files_item();

-----------------------------------------------------------自动催办数据库初始化----------------------------------------------------------------
select shard_icc_t_autourge();
select shard_icc_t_autourge_call();
select shard_icc_t_autourge_sms();

-- ----------------------------
-- Table structure for icc_t_110_visit_information_to12345
-- ----------------------------fshard_icc_t_alarm_log
DROP TABLE IF EXISTS "public"."icc_t_110_visit_information_to12345";
CREATE TABLE "public"."icc_t_110_visit_information_to12345" (
  "flowid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
  "jjdbh" varchar(50) COLLATE "pg_catalog"."default",
  "hfsj" varchar(50) COLLATE "pg_catalog"."default",
  "dhhfhsqk" varchar(2000) COLLATE "pg_catalog"."default",
  "cljgdm" varchar(10) COLLATE "pg_catalog"."default",
  "cljgmc" varchar(2000) COLLATE "pg_catalog"."default",
  "pushstatus" varchar(10) COLLATE "pg_catalog"."default",
  "pushtime" varchar(20) COLLATE "pg_catalog"."default",
  "xt_rksj" timestamp(0),
  "xt_gxsj" timestamp(0)
)
;
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."flowid" IS '12345标识';
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."jjdbh" IS '接警单编号';
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."hfsj" IS '回访时间';
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."dhhfhsqk" IS '电话回访核实情况';
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."cljgdm" IS '回访结果。字典项：1-满意；2-基本满意；3-不满意 ';
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."cljgmc" IS '回访内容';
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."pushstatus" IS '推送状态；0-未推送 1-已推送 2-已处理';
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."pushtime" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."xt_rksj" IS '入库时间';
COMMENT ON COLUMN "public"."icc_t_110_visit_information_to12345"."xt_gxsj" IS '更新时间';
COMMENT ON TABLE "public"."icc_t_110_visit_information_to12345" IS '12345转入110的回访信息';

-- ----------------------------
-- Table structure for icc_t_12345_basic_information_to110
-- ----------------------------
DROP TABLE IF EXISTS "public"."icc_t_12345_basic_information_to110";
CREATE TABLE "public"."icc_t_12345_basic_information_to110" (
  "flowid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
  "lsh" varchar(50) COLLATE "pg_catalog"."default",
  "jjybh" varchar(50) COLLATE "pg_catalog"."default",
  "jjdbh" varchar(50) COLLATE "pg_catalog"."default",
  "bjdh" varchar(100) COLLATE "pg_catalog"."default",
  "bjdhyhm" varchar(50) COLLATE "pg_catalog"."default",
  "lxdh" varchar(100) COLLATE "pg_catalog"."default",
  "bjdz" varchar(1000) COLLATE "pg_catalog"."default",
  "jqlbdm" varchar(50) COLLATE "pg_catalog"."default",
  "jqlbmc" varchar(50) COLLATE "pg_catalog"."default",
  "bjnr" varchar(5000) COLLATE "pg_catalog"."default",
  "gxdwdm" varchar(50) COLLATE "pg_catalog"."default",
  "gxdwmc" varchar(100) COLLATE "pg_catalog"."default",
  "gxdwdh" varchar(20) COLLATE "pg_catalog"."default",
  "bjsj" varchar(50) COLLATE "pg_catalog"."default",
  "tssj" varchar(50) COLLATE "pg_catalog"."default",
  "jjdwdm" varchar(50) COLLATE "pg_catalog"."default",
  "jjdwmc" varchar(100) COLLATE "pg_catalog"."default",
  "jjlx" varchar(10) COLLATE "pg_catalog"."default",
  "sfbm" varchar(10) COLLATE "pg_catalog"."default",
  "bjrxbdm" varchar(10) COLLATE "pg_catalog"."default",
  "title" varchar(200) COLLATE "pg_catalog"."default",
  "xzqhdm" varchar(20) COLLATE "pg_catalog"."default",
  "pushstatus" varchar(10) COLLATE "pg_catalog"."default",
  "pushtime" varchar(20) COLLATE "pg_catalog"."default",
  "acktime" varchar(20) COLLATE "pg_catalog"."default",
  "xt_rksj" timestamp(0),
  "xt_gxsj" timestamp(0),
  "seatno" varchar(20) COLLATE "pg_catalog"."default"
)
;
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."flowid" IS '12345标识';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."lsh" IS '流水号';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."jjybh" IS '接警员编号';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."jjdbh" IS '接警单编号';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."bjdh" IS '报警电话';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."bjdhyhm" IS '报警电话用户名';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."lxdh" IS '联系电话';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."bjdz" IS '报警地址';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."jqlbdm" IS '警情类别代码';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."jqlbmc" IS '警情类别名称';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."bjnr" IS '报警内容';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."gxdwdm" IS '管辖单位代码';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."gxdwmc" IS '管辖单位名称';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."gxdwdh" IS '管辖单位电话';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."bjsj" IS '报警时间';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."tssj" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."jjdwdm" IS '接警单位代码';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."jjdwmc" IS '接警单位名称';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."jjlx" IS '接警类型。默认值为110，字典项：01-110；02-122；00-12345 ';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."sfbm" IS '是否保密。默认值为不保密，字典项：0-否，1-是。';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."bjrxbdm" IS '默认值为未知，字典项：0-未知；1-男；2-女 ';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."title" IS '标题';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."xzqhdm" IS '行政区划代码';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."pushstatus" IS '推送状态；0-未推送 1-已推送 2-已处理';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."pushtime" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."acktime" IS '应答时间';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."xt_rksj" IS '入库时间';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."xt_gxsj" IS '更新时间';
COMMENT ON COLUMN "public"."icc_t_12345_basic_information_to110"."seatno" IS '推送坐席号';
COMMENT ON TABLE "public"."icc_t_12345_basic_information_to110" IS '12345警务类待办工单基础信息';

-- ----------------------------
-- Table structure for icc_t_12345_result_information_to110
-- ----------------------------
DROP TABLE IF EXISTS "public"."icc_t_12345_result_information_to110";
CREATE TABLE "public"."icc_t_12345_result_information_to110" (
  "flowid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
  "jjdbh" varchar(50) COLLATE "pg_catalog"."default",
  "fksj" varchar(50) COLLATE "pg_catalog"."default",
  "cjczqk" varchar(100) COLLATE "pg_catalog"."default",
  "jqcljgsm" varchar(4000) COLLATE "pg_catalog"."default",
  "fkybh" varchar(30) COLLATE "pg_catalog"."default",
  "fkyxm" varchar(50) COLLATE "pg_catalog"."default",
  "fkdwdm" varchar(50) COLLATE "pg_catalog"."default",
  "fkdwmc" varchar(100) COLLATE "pg_catalog"."default",
  "dyyy" varchar(10) COLLATE "pg_catalog"."default",
  "tssj" varchar(50) COLLATE "pg_catalog"."default",
  "pushstatus" varchar(10) COLLATE "pg_catalog"."default",
  "pushtime" varchar(20) COLLATE "pg_catalog"."default",
  "xt_rksj" timestamp(0),
  "xt_gxsj" timestamp(0)
)
;
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."flowid" IS '12345标识';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."jjdbh" IS '接警单编号';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."fksj" IS '反馈时间';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."cjczqk" IS '出警处置情况';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."jqcljgsm" IS '警情处理结果说明';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."fkybh" IS '反馈人员编号';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."fkyxm" IS '反馈人员姓名';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."fkdwdm" IS '反馈单位代码';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."fkdwmc" IS '反馈单位名称';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."dyyy" IS '接口调用原因。字典项：1-反馈，2-退回 3-回访';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."tssj" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."pushstatus" IS '推送状态；0-未推送 1-已推送 2-已处理';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."pushtime" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."xt_rksj" IS '入库时间';
COMMENT ON COLUMN "public"."icc_t_12345_result_information_to110"."xt_gxsj" IS '更新时间';
COMMENT ON TABLE "public"."icc_t_12345_result_information_to110" IS '110转入12345工单的反馈结果信息';

-- ----------------------------
-- Table structure for icc_t_12345_return_information_to110
-- ----------------------------
DROP TABLE IF EXISTS "public"."icc_t_12345_return_information_to110";
CREATE TABLE "public"."icc_t_12345_return_information_to110" (
  "flowid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
  "jjdbh" varchar(50) COLLATE "pg_catalog"."default",
  "handleusername" varchar(100) COLLATE "pg_catalog"."default",
  "handlebmname" varchar(100) COLLATE "pg_catalog"."default",
  "handleopinion" varchar(500) COLLATE "pg_catalog"."default",
  "handletime" varchar(20) COLLATE "pg_catalog"."default",
  "dyyy" varchar(10) COLLATE "pg_catalog"."default",
  "tssj" varchar(50) COLLATE "pg_catalog"."default",
  "pushstatus" varchar(10) COLLATE "pg_catalog"."default",
  "pushtime" varchar(20) COLLATE "pg_catalog"."default",
  "xt_rksj" timestamp(0),
  "xt_gxsj" timestamp(0)
)
;
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."flowid" IS '12345标识';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."jjdbh" IS '接警单编号';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."handleusername" IS '退回接警员';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."handlebmname" IS '退回部门(进行退回操作时，为110）';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."handleopinion" IS '退回原因';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."handletime" IS '退回时间';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."dyyy" IS '接口调用原因。字典项：1-反馈，2-退回 3-回访';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."tssj" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."pushstatus" IS '推送状态；0-未推送 1-已推送 2-已处理';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."pushtime" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."xt_rksj" IS '入库时间';
COMMENT ON COLUMN "public"."icc_t_12345_return_information_to110"."xt_gxsj" IS '更新时间';
COMMENT ON TABLE "public"."icc_t_12345_return_information_to110" IS '110转入12345工单退回信息';

-- ----------------------------
-- Table structure for icc_t_involved_party
-- ----------------------------
DROP TABLE IF EXISTS "public"."icc_t_involved_party";
CREATE TABLE "public"."icc_t_involved_party" (
	JQDSRDBH character varying(64) NOT NULL,
	JJDBH character varying(64),
	PJDBH character varying(64),
	FKDBH character varying(64),
	BJJQDSRDBM character varying(32),
	BJJJDBM character varying(32),
	BJPJDBM character varying(32),
	BJFKDBM character varying(32),
	XZQHDM character varying(12),
	XZQHMC character varying(100),
	JQDSRXM character varying(200),
	XBDM character varying(2),
	ZJDM character varying(32),
	ZJHM character varying(96),
	SF character varying(32),
	SFZDRY character varying(1),
	ZDRYSX character varying(128),
	HJXZQH character varying(6),
	HJXZ character varying(100),
	XZXZQH character varying(6),
	XZXZ character varying(100),
	GZDW character varying(128),
	ZY character varying(128),
	LXDH character varying(50),
	QTZJ character varying(32),
	SSWPXX character varying(4000),
	RKSJ timestamp(6) without time zone,
	GXSJ timestamp(6) without time zone,
	CSRQ timestamp(6) without time zone,
	SZQM character varying(512),
	SCBS character varying(1),
	CJZDBS character varying(50),
	GXZDBS character varying(50),
	GJ character varying(10),
	MZ character varying(10),
	WHCD character varying(10),
	HYZK character varying(10),
	ZZMM character varying(10),
	SFRDDB character varying(10),
	GJGZRY character varying(10),
	CJRZH character varying(64),
	CJRXM character varying(200),
	CJDW character varying(64),
	CJDWMC character varying(200),
	GXRZH character varying(64),
	GXRXM character varying(200),
	GXDW character varying(64),
	GXDWMC character varying(200)
)
;

ALTER TABLE ONLY public.icc_t_involved_party ADD CONSTRAINT icc_involved_party_pkey PRIMARY KEY (JQDSRDBH);
COMMENT ON COLUMN public.icc_t_involved_party.jqdsrdbh IS '警情当事人单编号';
COMMENT ON COLUMN public.icc_t_involved_party.jjdbh IS '接警单编号';
COMMENT ON COLUMN public.icc_t_involved_party.pjdbh IS '派警单编号';
COMMENT ON COLUMN public.icc_t_involved_party.fkdbh IS '反馈单编号';
COMMENT ON COLUMN public.icc_t_involved_party.bjjqdsrdbm IS '部级警情当事人单编码';
COMMENT ON COLUMN public.icc_t_involved_party.bjjjdbm IS '部级接警单编码';
COMMENT ON COLUMN public.icc_t_involved_party.bjpjdbm IS '部级派警单编码';
COMMENT ON COLUMN public.icc_t_involved_party.bjfkdbm IS '部级反馈单编码';
COMMENT ON COLUMN public.icc_t_involved_party.xzqhdm IS '行政区划代码';
COMMENT ON COLUMN public.icc_t_involved_party.xzqhmc IS '行政区划名称';
COMMENT ON COLUMN public.icc_t_involved_party.jqdsrxm IS '警情当事人姓名';
COMMENT ON COLUMN public.icc_t_involved_party.xbdm IS '性别代码';
COMMENT ON COLUMN public.icc_t_involved_party.zjdm IS '证件代码';
COMMENT ON COLUMN public.icc_t_involved_party.zjhm IS '证件号码';
COMMENT ON COLUMN public.icc_t_involved_party.sf IS '警情当事人身份';
COMMENT ON COLUMN public.icc_t_involved_party.sfzdry IS '是否重点人员';
COMMENT ON COLUMN public.icc_t_involved_party.zdrysx IS '重点人员属性';
COMMENT ON COLUMN public.icc_t_involved_party.hjxzqh IS '户籍行政区划';
COMMENT ON COLUMN public.icc_t_involved_party.hjxz IS '户籍详址';
COMMENT ON COLUMN public.icc_t_involved_party.xzxzqh IS '现住行政区划';
COMMENT ON COLUMN public.icc_t_involved_party.xzxz IS '现住详址';
COMMENT ON COLUMN public.icc_t_involved_party.gzdw IS '工作单位';
COMMENT ON COLUMN public.icc_t_involved_party.zy IS '职业';
COMMENT ON COLUMN public.icc_t_involved_party.lxdh IS '联系电话';
COMMENT ON COLUMN public.icc_t_involved_party.qtzj IS '其他证件';
COMMENT ON COLUMN public.icc_t_involved_party.sswpxx IS '损失物品信息';
COMMENT ON COLUMN public.icc_t_involved_party.rksj IS '入库时间';
COMMENT ON COLUMN public.icc_t_involved_party.gxsj IS '更新时间';
COMMENT ON COLUMN public.icc_t_involved_party.csrq IS '出生日期';
COMMENT ON COLUMN public.icc_t_involved_party.szqm IS '数字签名';
COMMENT ON COLUMN public.icc_t_involved_party.scbs IS '删除标识';
COMMENT ON COLUMN public.icc_t_involved_party.cjzdbs IS '创建终端标识';
COMMENT ON COLUMN public.icc_t_involved_party.gxzdbs IS '更新终端标识';
COMMENT ON COLUMN public.icc_t_involved_party.gj IS '国籍';
COMMENT ON COLUMN public.icc_t_involved_party.mz IS '民族';
COMMENT ON COLUMN public.icc_t_involved_party.whcd IS '文化程度';
COMMENT ON COLUMN public.icc_t_involved_party.hyzk IS '婚姻状况';
COMMENT ON COLUMN public.icc_t_involved_party.zzmm IS '政治面貌';
COMMENT ON COLUMN public.icc_t_involved_party.sfrddb IS '是否人大代表';
COMMENT ON COLUMN public.icc_t_involved_party.gjgzry IS '国家工作人员';
COMMENT ON COLUMN public.icc_t_involved_party.cjrzh IS '登记人证号';
COMMENT ON COLUMN public.icc_t_involved_party.cjrxm IS '登记人姓名';
COMMENT ON COLUMN public.icc_t_involved_party.cjdw IS '登记单位';
COMMENT ON COLUMN public.icc_t_involved_party.cjdwmc IS '登记单位名称';
COMMENT ON COLUMN public.icc_t_involved_party.gxrzh IS '修改人证号';
COMMENT ON COLUMN public.icc_t_involved_party.gxrxm IS '修改人姓名';
COMMENT ON COLUMN public.icc_t_involved_party.gxdw IS '修改单位';

-- ----------------------------
-- Table structure for icc_t_12345_visit_information_to110
-- ----------------------------
DROP TABLE IF EXISTS "public"."icc_t_12345_visit_information_to110";
CREATE TABLE "public"."icc_t_12345_visit_information_to110" (
  "flowid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
  "jjdbh" varchar(50) COLLATE "pg_catalog"."default",
  "hfsj" varchar(50) COLLATE "pg_catalog"."default",
  "dhhfhsqk" varchar(2000) COLLATE "pg_catalog"."default",
  "cljgdm" varchar(10) COLLATE "pg_catalog"."default",
  "cljgmc" varchar(2000) COLLATE "pg_catalog"."default",
  "dyyy" varchar(10) COLLATE "pg_catalog"."default",
  "tssj" varchar(50) COLLATE "pg_catalog"."default",
  "pushstatus" varchar(10) COLLATE "pg_catalog"."default",
  "pushtime" varchar(20) COLLATE "pg_catalog"."default",
  "xt_rksj" timestamp(0),
  "xt_gxsj" timestamp(0)
)
;
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."flowid" IS '12345标识';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."jjdbh" IS '接警单编号';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."hfsj" IS '回访时间';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."dhhfhsqk" IS '电话回访核实情况';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."cljgdm" IS '回访结果。字典项：1-满意；2-基本满意；3-不满意 ';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."cljgmc" IS '回访内容';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."dyyy" IS '接口调用原因。字典项：1-反馈，2-退回 3-回访';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."tssj" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."pushstatus" IS '推送状态；0-未推送 1-已推送 2-已处理';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."pushtime" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."xt_rksj" IS '入库时间';
COMMENT ON COLUMN "public"."icc_t_12345_visit_information_to110"."xt_gxsj" IS '更新时间';
COMMENT ON TABLE "public"."icc_t_12345_visit_information_to110" IS '110转入12345工单回访信息';

-- ----------------------------
-- Primary Key structure for table icc_t_110_visit_information_to12345
-- ----------------------------
ALTER TABLE "public"."icc_t_110_visit_information_to12345" ADD CONSTRAINT "icc_t_12345_visit_information_to110_copy1_pkey" PRIMARY KEY ("flowid");

-- ----------------------------
-- Primary Key structure for table icc_t_12345_basic_information_to110
-- ----------------------------
ALTER TABLE "public"."icc_t_12345_basic_information_to110" ADD CONSTRAINT "icc1_pkey" PRIMARY KEY ("flowid");

-- ----------------------------
-- Primary Key structure for table icc_t_12345_result_information_to110
-- ----------------------------
ALTER TABLE "public"."icc_t_12345_result_information_to110" ADD CONSTRAINT "icc2_pkey" PRIMARY KEY ("flowid");

-- ----------------------------
-- Primary Key structure for table icc_t_12345_return_information_to110
-- ----------------------------
ALTER TABLE "public"."icc_t_12345_return_information_to110" ADD CONSTRAINT "icc4_pkey" PRIMARY KEY ("flowid");

-- ----------------------------
-- Primary Key structure for table icc_t_12345_visit_information_to110
-- ----------------------------
ALTER TABLE "public"."icc_t_12345_visit_information_to110" ADD CONSTRAINT "icc3_pkey" PRIMARY KEY ("flowid");


DROP TABLE IF EXISTS "public"."icc_t_bllstatus";
CREATE TABLE icc_t_bllstatus (
    id character varying(64) NOT NULL,
    jjdbh character varying(64),
    pjdbh character varying(64),
    owner character varying(200),
    ownertype character varying(64),
	status character varying(64),
    gxsj timestamp(6) without time zone,
    cjsj timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_bllstatus ADD CONSTRAINT icc_t_bllstatus_pkey PRIMARY KEY (id);

DROP TABLE IF EXISTS "public"."icc_t_sync_log";
CREATE TABLE icc_t_sync_log (
    guid character varying(64) NOT NULL,
    sync_type character varying(64),
    sync_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_sync_log ADD CONSTRAINT icc_t_sync_log_pkey PRIMARY KEY (guid);

DROP TABLE IF EXISTS "public"."icc_t_alarm";
CREATE TABLE icc_t_alarm (
    id character varying(64) NOT NULL,
    merge_id character varying(64),
    title character varying(1024),
    content text,
    "time" timestamp(6) without time zone NOT NULL,
    actual_occur_time timestamp(6) without time zone,
    addr text,
    longitude double precision,
    latitude double precision,
    state character varying(64),
    level character varying(64),
    source_type character varying(64),
    source_id character varying(64),
    handle_type character varying(64),
    first_type character varying(64),
    second_type character varying(64),
    third_type character varying(64),
    fourth_type character varying(64),
    vehicle_no character varying(64),
    vehicle_type character varying(64),
    symbol_code character varying(64),
    symbol_addr character varying(64),
    fire_building_type character varying(64),
    event_type text,
    called_no_type character varying(64),
    actual_called_no_type character varying(64),
    caller_no character varying(64),
    caller_name character varying(1024),
    caller_addr text,
    caller_id character varying(64),
    caller_id_type character varying(64),
    caller_gender character varying(64),
    caller_age character varying(64),
    caller_birthday character varying(64),
    contact_no character varying(64),
    contact_name character varying(2048),
    contact_addr text,
    contact_id character varying(64),
    contact_id_type character varying(64),
    contact_gender character varying(64),
    contact_age character varying(64),
    contact_birthday character varying(64),
    admin_dept_district_code character varying(256),
    admin_dept_code character varying(256),
    admin_dept_name character varying(2048),
    receipt_dept_district_code character varying(256),
    receipt_dept_code character varying(256),
    receipt_dept_name character varying(2048),
    leader_code character varying(64),
    leader_name character varying(2048),
    receipt_code character varying(64),
    receipt_name character varying(2048),
    receipt_seatno character varying(64),
    is_merge character varying(64),
    dispatch_suggestion text,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone,
    is_feedback character varying(64),
    is_visitor character varying(64),
	city_code character varying(512),
	first_submit_time timestamp(6) without time zone,
	YWWXWZ character varying(8),
	YWBZXL character varying(8),
	BKRYQKSM text,
	SSRYQKSM text,
	SWRYQKSM text,
	SFSWYBJ character varying(8),
	FXDWJD double precision,
	FXDWWD double precision,
	BCJJNR text,
	YJJYDJDM character varying(64),
	SFSWHCL character varying(8),
	JQGJZ text,
	BM character varying(8),
	is_privacy character varying(64),
	is_delete character varying(64),
	remark text
);
ALTER TABLE ONLY icc_t_alarm ADD CONSTRAINT icc_t_alarm_pkey PRIMARY KEY (id);
CREATE INDEX idx_t_alarm_addr ON icc_t_alarm USING gin (addr gin_trgm_ops);
CREATE INDEX idx_t_alarm_caller_no ON icc_t_alarm USING gin (caller_no gin_trgm_ops);
CREATE INDEX idx_t_alarm_receipt_seatno ON icc_t_alarm USING gin (content gin_trgm_ops);
CREATE INDEX idx_t_alarm_content ON icc_t_alarm USING gin (content gin_trgm_ops);
CREATE INDEX idx_t_alarm_id ON icc_t_alarm USING gin (id gin_trgm_ops);
CREATE INDEX idx_t_alarm_source_id ON icc_t_alarm USING btree (source_id);
CREATE INDEX idx_t_alarm_ten_in_one ON icc_t_alarm USING btree (state, level, handle_type, first_type, second_type, third_type, called_no_type, admin_dept_code, receipt_dept_code, receipt_code);
CREATE INDEX idx_t_alarm_time ON icc_t_alarm USING brin ("time") WITH (pages_per_range=1);
create index icc_t_alarm_uptime on icc_t_alarm (update_time);
create index icc_t_alarm_crttime on icc_t_alarm (create_time);

DROP TABLE IF EXISTS "public"."icc_t_alarm_book";
CREATE TABLE icc_t_alarm_book (
    guid character varying(64) NOT NULL,
    id character varying(64) NOT NULL,
    merge_id character varying(64),
    title character varying(64),
    content text,
    "time" timestamp(6) without time zone NOT NULL,
    actual_occur_time timestamp(6) without time zone,
    addr text,
    longitude double precision,
    latitude double precision,
    state character varying(64),
    level character varying(64),
    source_type character varying(64),
    source_id character varying(64),
    handle_type character varying(64),
    first_type character varying(64),
    second_type character varying(64),
    third_type character varying(64),
    fourth_type character varying(64),
    vehicle_no character varying(64),
    vehicle_type character varying(64),
    symbol_code character varying(64),
    symbol_addr character varying(64),
    fire_building_type character varying(64),
    event_type text,
    called_no_type character varying(64),
    actual_called_no_type character varying(64),
    caller_no character varying(64),
    caller_name character varying(2048),
    caller_addr text,
    caller_id character varying(64),
    caller_id_type character varying(64),
    caller_gender character varying(64),
    caller_age character varying(64),
    caller_birthday character varying(64),
    contact_no character varying(64),
    contact_name character varying(2048),
    contact_addr text,
    contact_id character varying(64),
    contact_id_type character varying(64),
    contact_gender character varying(64),
    contact_age character varying(64),
    contact_birthday character varying(64),
    admin_dept_district_code character varying(64),
    admin_dept_code character varying(64),
    admin_dept_name character varying(2048),
    receipt_dept_district_code character varying(64),
    receipt_dept_code character varying(64),
    receipt_dept_name character varying(2048),
    leader_code character varying(64),
    leader_name character varying(2048),
    receipt_code character varying(64),
    receipt_name character varying(2048),
    receipt_seatno character varying(64),
    is_merge character varying(64),
    dispatch_suggestion text,
    flag character varying(64),
	city_code character varying(512),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone,
	is_privacy character varying(64),
	is_delete character varying(64),
	remark text
);
ALTER TABLE ONLY icc_t_alarm_book ADD CONSTRAINT icc_t_alarm_book_pkey PRIMARY KEY (guid, flag);
CREATE INDEX idx_t_alarm_book_id ON icc_t_alarm_book USING gin (id gin_trgm_ops);
CREATE INDEX idx_t_alarm_book_add ON icc_t_alarm_book USING gin (addr gin_trgm_ops);
CREATE INDEX idx_t_alarm_book_caller_no ON icc_t_alarm_book USING gin (caller_no gin_trgm_ops);
CREATE INDEX idx_t_alarm_book_content ON icc_t_alarm_book USING gin (content gin_trgm_ops);
CREATE INDEX idx_t_alarm_book_receipt_seatno ON icc_t_alarm_book USING btree (receipt_seatno);
CREATE INDEX idx_t_alarm_book_source_id ON icc_t_alarm_book USING btree (source_id);
CREATE INDEX idx_t_alarm_book_ten_in_one ON icc_t_alarm_book USING gin (state gin_trgm_ops, level gin_trgm_ops, handle_type gin_trgm_ops, first_type gin_trgm_ops, second_type gin_trgm_ops, third_type gin_trgm_ops, called_no_type gin_trgm_ops, admin_dept_code gin_trgm_ops, receipt_dept_code gin_trgm_ops, receipt_code gin_trgm_ops);
CREATE INDEX idx_t_alarm_book_time ON icc_t_alarm_book USING brin ("time") WITH (pages_per_range=1);

--新接警单备份表
DROP TABLE IF EXISTS "public"."icc_t_jjdb_book";
CREATE TABLE public.icc_t_jjdb_book (
    guid character varying(64) NOT NULL,
    XZQHDM character varying(256),
	JJDWDM character varying(256),
    JJDBH character varying(64) NOT NULL,
	GLZJJDBH character varying(64),
	JJLX character varying(64),
	JQLYFS character varying(64),
	LHLX character varying(64),
	JJLYH character varying(64), 
	JJYBH character varying(64),
	JJYXM character varying(2048),
	BJSJ timestamp(6) without time zone NOT NULL,
	JJSJ timestamp(6) without time zone, 
	JJWCSJ timestamp(6) without time zone,
    BJDH character varying(64),
	BJDHYHM character varying(64),	
    BJDHYHDZ text,
    BJRMC character varying(1024),
    BJRXBDM character varying(64),
    LXDH character varying(64),
    BJRZJDM character varying(64),	
    BJRZJHM character varying(64),	
	BJDZ text,
    JQDZ text,	
	BJNR text,
    GXDWDM character varying(256),
    JQLBDM character varying(64),
    JQLXDM character varying(64),
    JQXLDM character varying(64),
    JQZLDM character varying(64),
    TZDBH character varying(64),
	ZARS integer,
	YWCWQ character varying(64),
	YWWXWZ character varying(64),
	YWBZXL character varying(64),
	BKRYQKSM text,
	SSRYQKSM text,
	SWRYQKSM text,
	SFSWYBJ character varying(64),
    BJRXZB double precision,
    BJRYZB double precision,	
	FXDWJD double precision,
	FXDWWD double precision,
	BCJJNR text,	
    JQDJDM character varying(64),	
    JQCLZTDM character varying(64),
	YJJYDJDM character varying(64),
    SJCHPZLDM character varying(64),	
    SJCPH character varying(64),
	SFSWHCL character varying(64),	
    CJSJ timestamp(6) without time zone,	
    GXSJ timestamp(6) without time zone,
    JQBQ character varying(1024),
    SFBM character varying(64),
	JJFS character varying(64),
    GXDWMC character varying(2048),
	GXDWDMBS character varying(64),
    JJDWMC character varying(2048),
    JJDWDMBS character varying(64),
	SCBS character varying(64),
    WXBS character varying(64),
	DQYWZT character varying(64),
    JJXWH character varying(64),
    HBBS character varying(64),
    CJRY character varying(64),
    GXRY character varying(64),
    FKBS character varying(64),
    HFBS character varying(64),
    flag character varying(64),
    GLLX varchar(10),
	CJZDBS character varying(50),
	GXZDBS character varying(50)
);

COMMENT ON COLUMN public.icc_t_jjdb_book.guid
    IS '因为guid是从其它表来的，有重复，guid与flag联合主键';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.XZQHDM
    IS '行政区划代码(警情所在地行政区划代码，标识到县、市一级。如果县级报警也在市级指挥中心接警，生成警情的行政区划应是报警所在市的行政区划)';

COMMENT ON COLUMN public.icc_t_jjdb_book.JJDWDM
    IS '接警单位代码(接警单位一般是当地公安机关110报警服务台，也包括当地自接警单位)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JJDBH 
    IS '接警单编号(接警单编号，产生规则为警情行政区划代码+系统接警单编号，
	以避免不同地区产生同样编号；唯一号，主索引键，并用于关联派警单、反馈单。)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.GLZJJDBH 
    IS '关联主接警单编号(当前接警单所关联的主接警单的编号。)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JJLX
    IS '接警类型(接警类型代码，区分警情属于110、122、自接警和其他接警等)';

COMMENT ON COLUMN public.icc_t_jjdb_book.JQLYFS
    IS '警情来源方式(警情来源方式，用于说明警情接入的方式，包括电话报警、短信报警、网络报警等。)';

COMMENT ON COLUMN public.icc_t_jjdb_book.LHLX
    IS '来话类型(来话类型代码，用来描述该来话的基本分类。)';

COMMENT ON COLUMN public.icc_t_jjdb_book.JJLYH
    IS '接警录音号(接警录音号，接警时由录音系统自动产生，接处警系统自动关联。)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JJYBH
    IS '接警员编号(接警员编号，用来标识接警员身份。)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JJYXM
    IS '接警员姓名';

COMMENT ON COLUMN public.icc_t_jjdb_book.BJSJ
    IS '报警时间(电话报警时间为排队调度机将报警电话分配到接警台的时间；其他报警方式时间为实际报警时间)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JJSJ
    IS '接警时间(电话报警时为接警员摘电话机应答时间；其余报警方式时为录入电脑开始时间)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JJWCSJ
    IS '接警完成时间(语音报警时为接警员挂断电话机时间，其他报警方式时为保存接警单退出时间)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.BJDH
    IS '报警电话(报警电话号码，三字段信息之一)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.BJDHYHM
    IS '报警电话用户名(报警电话用户姓名，三字段信息之一，呼入时，由查号系统自动产生)';

COMMENT ON COLUMN public.icc_t_jjdb_book.BJDHYHDZ
    IS '报警电话用户地址(报警电话用户地址，三字段信息之一，呼入时，由查号系统自动产生)';

COMMENT ON COLUMN public.icc_t_jjdb_book.BJRMC
    IS '报警人名称(包括互联网报警网名或技防网点名称、设备编号等），考虑外国人报警，留200位，默认为“匿名”)';
		
COMMENT ON COLUMN public.icc_t_jjdb_book.BJRXBDM
    IS '报警人性别代码';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.LXDH
    IS '联系电话(报警人联系电话，默认为报警电话)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.BJRZJDM
    IS '报警人证件代码(报警人证件类型名称)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.BJRZJHM
    IS '报警人证件号码(报警人有效证件号码)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.BJDZ
    IS '报警地址(报警人所在地址、单位名等。)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JQDZ
    IS '警情地址(警情发生地的地址、单位名等)';

COMMENT ON COLUMN public.icc_t_jjdb_book.BJNR 
    IS '报警内容(报警内容的详细文字描述)';

COMMENT ON COLUMN public.icc_t_jjdb_book.GXDWDM
    IS '管辖单位代码(警情管辖受理单位的机构代码)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JQLBDM
    IS '警情类别代码(警情类别代码，用来描述警情性质的一级类型代码)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JQLXDM
    IS '警情类型代码(警情类型代码，用来描述警情性质的二级类型代码)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JQXLDM
    IS '警情细类代码(警情细类代码，用来描述警情性质的三级类型代码)';

COMMENT ON COLUMN public.icc_t_jjdb_book.JQZLDM
    IS '警情子类代码(警情子类代码，用来描述警情性质的四级类型代码)';

COMMENT ON COLUMN public.icc_t_jjdb_book.TZDBH
    IS '特征点编号(特征点编号，如路灯杆、视频安装点等辅助定位设施的编号)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.ZARS
    IS '作案人数(违法犯罪嫌疑人数量)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.YWCWQ
    IS '有无持武器(0：无，1：有，默认为0)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.YWWXWZ
    IS '有无危险物质(0：无，1：有，默认为0)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.YWBZXL
    IS '有无爆炸/泄漏(0：无，1：有，默认为0)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.BKRYQKSM
    IS '被困人员情况说明';

COMMENT ON COLUMN public.icc_t_jjdb_book.SSRYQKSM
    IS '受伤人员情况说明';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.SWRYQKSM
    IS '死亡人员情况说明';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.SFSWYBJ
    IS '是否是外语报警';

COMMENT ON COLUMN public.icc_t_jjdb_book.BJRXZB
    IS '报警人定位X坐标';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.BJRYZB
    IS '报警人定位Y坐标';

COMMENT ON COLUMN public.icc_t_jjdb_book.FXDWJD
    IS '反向定位X坐标';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.FXDWWD
    IS '反向定位Y坐标';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.BCJJNR
    IS '补充接警内容';	
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JQDJDM
    IS '警情等级代码';	
 
COMMENT ON COLUMN public.icc_t_jjdb_book.JQCLZTDM
    IS '警情处理状态代码(记录该条警情目前的业务状态)';	
	
COMMENT ON COLUMN public.icc_t_jjdb_book.YJJYDJDM
    IS '应急救援等级代码';

COMMENT ON COLUMN public.icc_t_jjdb_book.SJCHPZLDM
    IS '涉及车号牌种类代码';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.SJCPH
    IS '涉及车牌号';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.SFSWHCL
    IS '是否是危化车辆(0：否，1：是，默认为0)';

COMMENT ON COLUMN public.icc_t_jjdb_book.CJSJ
    IS '创建时间(记录创建的时间)';

COMMENT ON COLUMN public.icc_t_jjdb_book.GXSJ
    IS '更新时间(记录新增、变更的时间)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JQBQ
    IS '警情标签(警情标签是对警情内容关键要素的提取和标注。)';

COMMENT ON COLUMN public.icc_t_jjdb_book.SFBM
    IS '是否保密(0：否，1：是，默认为0)';  
   	
COMMENT ON COLUMN public.icc_t_jjdb_book.JJFS
    IS '接警服务(区分是vcs自接、icc接警、mpa接警)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.GXDWMC
    IS '管辖单位名称';	

COMMENT ON COLUMN public.icc_t_jjdb_book.GXDWDMBS
    IS '管辖单位短码';	

COMMENT ON COLUMN public.icc_t_jjdb_book.JJDWMC
    IS '接警单位名称';	

COMMENT ON COLUMN public.icc_t_jjdb_book.JJDWDMBS
    IS '接警单位短码';	
	
COMMENT ON COLUMN public.icc_t_jjdb_book.SCBS
    IS '删除标记位';	

COMMENT ON COLUMN public.icc_t_jjdb_book.WXBS
    IS '无效警单标记位';	
	
COMMENT ON COLUMN public.icc_t_jjdb_book.DQYWZT
    IS '当前业务状态(vcs业务状态)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.JJXWH
    IS '警情接警人席位号';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.HBBS
    IS '是否合并(是否合并1：合并，0：未合并)';	
	
COMMENT ON COLUMN public.icc_t_jjdb_book.CJRY
    IS '创建人';	
	
COMMENT ON COLUMN public.icc_t_jjdb_book.GXRY
    IS '更新人(修改人,取最后一次修改值修改时间,取最后一次修改值)';

COMMENT ON COLUMN public.icc_t_jjdb_book.HFBS
    IS '是否回访';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.FKBS
    IS '是否反馈';

COMMENT ON COLUMN public.icc_t_jjdb_book.flag
    IS '修改标记(before, after)';
	
COMMENT ON COLUMN public.icc_t_jjdb_book.CJZDBS
    IS '创建终端标示';

COMMENT ON COLUMN public.icc_t_jjdb_book.GXZDBS
    IS '更新终端标示';
	
ALTER TABLE ONLY icc_t_jjdb_book ADD CONSTRAINT icc_t_jjdb_book_pkey PRIMARY KEY (guid, flag);
CREATE INDEX idx_t_jjdb_book_id ON icc_t_jjdb_book USING gin (jjdbh gin_trgm_ops);

CREATE INDEX idx_t_jjdb_book_addr ON icc_t_jjdb_book USING gin (jqdz gin_trgm_ops);
CREATE INDEX idx_t_jjdb_book_caller_no ON icc_t_jjdb_book USING gin (bjdh gin_trgm_ops);
--CREATE INDEX idx_t_jjdb_receipt_seatno ON icc_t_jjdb_book USING gin (content gin_trgm_ops);
CREATE INDEX idx_t_jjdb_book_content ON icc_t_jjdb_book USING gin (bjnr gin_trgm_ops);

CREATE INDEX idx_t_jjdb_book_source_id ON icc_t_jjdb_book USING btree (jjlyh);
CREATE INDEX idx_t_jjdb_book_ten_in_one ON icc_t_jjdb_book USING btree (jqclztdm, jqdjdm, lhlx, jqlbdm, jqlxdm, jqxldm, jjlx, gxdwdm, jjdwdm, jjybh);
CREATE INDEX idx_t_jjdb_book_time ON icc_t_jjdb_book USING brin (bjsj) WITH (pages_per_range=1);


--新修改的警情表  end
DROP TABLE IF EXISTS "public"."icc_t_alarm_edit_lock";
CREATE TABLE icc_t_alarm_edit_lock (
    id character varying(64) NOT NULL,
    user_code character varying(64),
    lock_time integer,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_alarm_edit_lock ADD CONSTRAINT icc_t_alarm_edit_lock_pkey PRIMARY KEY (id);
DROP TABLE IF EXISTS "public"."icc_t_alarm_feedback";
CREATE TABLE icc_t_alarm_feedback (
    id character varying(64) NOT NULL,
    alarm_id character varying(64) NOT NULL,
    process_id character varying(64) NOT NULL,
    result_type text,
    result_content text,
    leader_instruction text,
    state character varying(64),
    time_edit timestamp(6) without time zone,
    time_submit timestamp(6) without time zone,
    time_arrived timestamp(6) without time zone,
    time_signed timestamp(6) without time zone,
    time_police_dispatch timestamp(6) without time zone,
    time_police_arrived timestamp(6) without time zone,
    actual_occur_time timestamp(6) without time zone,
    actual_occur_addr text,
    feedback_dept_district_code character varying(64),
    feedback_dept_code character varying(64),
    feedback_dept_name character varying(2048),
    feedback_code character varying(64),
    feedback_name character varying(2048),
    feedback_leader_code character varying(64),
    feedback_leader_name character varying(64),
    process_dept_district_code character varying(64),
    process_dept_code character varying(64),
    process_dept_name character varying(2048),
    process_code character varying(64),
    process_name character varying(2048),
    process_leader_code character varying(64),
    process_leader_name character varying(2048),
    dispatch_dept_district_code character varying(64),
    dispatch_dept_code character varying(64),
    dispatch_dept_name character varying(2048),
    dispatch_code character varying(64),
    dispatch_name character varying(2048),
    dispatch_leader_code character varying(64),
    dispatch_leader_name character varying(2048),
    person_id character varying(64),
    person_id_type character varying(64),
    person_nationality character varying(64),
    person_name character varying(64),
    person_slave_id character varying(64),
    person_slave_id_type character varying(64),
    person_slave_nationality character varying(64),
    person_slave_name character varying(2048),
    alarm_called_no_type character varying(64),
    alarm_first_type character varying(64),
    alarm_second_type character varying(64),
    alarm_third_type character varying(64),
    alarm_fourth_type character varying(64),
    alarm_addr_dept_name character varying(2048),
    alarm_addr_first_type character varying(64),
    alarm_addr_second_type character varying(64),
    alarm_addr_third_type character varying(64),
    alarm_longitude double precision,
    alarm_latitude double precision,
    alarm_region_type character varying(64),
    alarm_location_type character varying(64),
    people_num_capture integer,
    people_num_rescue integer,
    people_num_slight_injury integer,
    people_num_serious_injury integer,
    people_num_death integer,
    police_num_dispatch integer,
    police_car_num_dispatch integer,
    economy_loss double precision,
    retrieve_economy_loss double precision,
    fire_put_out_time timestamp(6) without time zone,
    fire_building_first_type character varying,
    fire_building_second_type character varying(64),
    fire_building_third_type character varying(64),
    fire_source_type character varying(64),
    fire_region_type character varying(64),
    fire_cause_first_type character varying(64),
    fire_cause_second_type character varying(64),
    fire_cause_third_type character varying(64),
    fire_area double precision,
    traffic_road_level character varying(64),
    traffic_accident_level character varying(64),
    traffic_vehicle_no character varying(64),
    traffic_vehicle_type character varying(64),
    traffic_slave_vehicle_no character varying(64),
    traffic_slave_vehicle_type character varying(64),
    event_type text,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_alarm_feedback ADD CONSTRAINT icc_t_alarm_feedback_pkey PRIMARY KEY (process_id, id);
CREATE INDEX idx_t_feedback_five_in_one ON icc_t_alarm_feedback USING btree (alarm_id, alarm_first_type, alarm_second_type, alarm_third_type, alarm_fourth_type);
CREATE INDEX idx_t_feedback_id ON icc_t_alarm_feedback USING btree (id);
CREATE INDEX idx_t_feedback_result_content ON icc_t_alarm_feedback USING btree (result_content);
CREATE INDEX idx_t_feedback_update_time ON icc_t_alarm_feedback USING brin (update_time) WITH (pages_per_range='1');
create index icc_t_alarm_feedback_crttime on icc_t_alarm_feedback(create_time);
create index icc_t_alarm_feedback_uptime on icc_t_alarm_feedback(update_time);

DROP TABLE IF EXISTS "public"."icc_t_alarm_feedback_book";
CREATE TABLE icc_t_alarm_feedback_book (
    guid character varying(64) NOT NULL,
    id character varying(64) NOT NULL,
    alarm_id character varying(64) NOT NULL,
    process_id character varying(64) NOT NULL,
    result_type text,
    result_content text,
    leader_instruction text,
    state character varying(64),
    time_edit timestamp(6) without time zone,
    time_submit timestamp(6) without time zone,
    time_arrived timestamp(6) without time zone,
    time_signed timestamp(6) without time zone,
    time_police_dispatch timestamp(6) without time zone,
    time_police_arrived timestamp(6) without time zone,
    actual_occur_time timestamp(6) without time zone,
    actual_occur_addr text,
    feedback_dept_district_code character varying(64),
    feedback_dept_code character varying(64),
    feedback_dept_name character varying(2048),
    feedback_code character varying(64),
    feedback_name character varying(2048),
    feedback_leader_code character varying(64),
    feedback_leader_name character varying(2048),
    process_dept_district_code character varying(64),
    process_dept_code character varying(64),
    process_dept_name character varying(2048),
    process_code character varying(64),
    process_name character varying(64),
    process_leader_code character varying(64),
    process_leader_name character varying(2048),
    dispatch_dept_district_code character varying(64),
    dispatch_dept_code character varying(64),
    dispatch_dept_name character varying(2048),
    dispatch_code character varying(64),
    dispatch_name character varying(2048),
    dispatch_leader_code character varying(64),
    dispatch_leader_name character varying(2048),
    person_id character varying(64),
    person_id_type character varying(64),
    person_nationality character varying(64),
    person_name character varying(2048),
    person_slave_id character varying(64),
    person_slave_id_type character varying(64),
    person_slave_nationality character varying(64),
    person_slave_name character varying(2048),
    alarm_called_no_type character varying(64),
    alarm_first_type character varying(64),
    alarm_second_type character varying(64),
    alarm_third_type character varying(64),
    alarm_fourth_type character varying(64),
    alarm_addr_dept_name character varying(2048),
    alarm_addr_first_type character varying(64),
    alarm_addr_second_type character varying(64),
    alarm_addr_third_type character varying(64),
    alarm_longitude double precision,
    alarm_latitude double precision,
    alarm_region_type character varying(64),
    alarm_location_type character varying(64),
    people_num_capture integer,
    people_num_rescue integer,
    people_num_slight_injury integer,
    people_num_serious_injury integer,
    people_num_death integer,
    police_num_dispatch integer,
    police_car_num_dispatch integer,
    economy_loss double precision,
    retrieve_economy_loss double precision,
    fire_put_out_time timestamp(6) without time zone,
    fire_building_first_type character varying,
    fire_building_second_type character varying(64),
    fire_building_third_type character varying(64),
    fire_source_type character varying(64),
    fire_region_type character varying(64),
    fire_cause_first_type character varying(64),
    fire_cause_second_type character varying(64),
    fire_cause_third_type character varying(64),
    fire_area double precision,
    traffic_road_level character varying(64),
    traffic_accident_level character varying(64),
    traffic_vehicle_no character varying(64),
    traffic_vehicle_type character varying(64),
    traffic_slave_vehicle_no character varying(64),
    traffic_slave_vehicle_type character varying(64),
    event_type text,
    flag character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_alarm_feedback_book ADD CONSTRAINT icc_t_alarm_feedback_book_pkey PRIMARY KEY (guid,flag);
CREATE INDEX idx_t_feedback_book_id ON icc_t_alarm_feedback_book USING btree (id);
CREATE INDEX idx_t_feedback_book_five_in_one ON icc_t_alarm_feedback_book USING btree (alarm_id, alarm_first_type, alarm_second_type, alarm_third_type, alarm_fourth_type);
CREATE INDEX idx_t_feedback_book_result_content ON icc_t_alarm_feedback_book USING btree (result_content);
CREATE INDEX idx_t_feedback_book_update_time ON icc_t_alarm_feedback_book USING brin (update_time) WITH (pages_per_range='1');

DROP TABLE IF EXISTS "public"."icc_t_alarm_process";
CREATE TABLE icc_t_alarm_process (
    id character varying(64),
    alarm_id character varying(64),
    state character varying(64),
    time_edit timestamp(6) without time zone,
    time_submit timestamp(6) without time zone,
    time_arrived timestamp(6) without time zone,
    time_signed timestamp(6) without time zone,
    time_feedback timestamp(6) without time zone,
    is_need_feedback character varying(64),
    dispatch_dept_district_code character varying(64),
    dispatch_dept_code character varying(64),
    dispatch_dept_name character varying(2048),
    dispatch_code character varying(64),
    dispatch_name character varying(2048),
    dispatch_leader_code character varying(64),
    dispatch_leader_name character varying(2048),
    dispatch_suggestion text,
    dispatch_leader_instruction text,
    process_dept_district_code character varying(64),
    process_dept_code character varying(64),
    process_dept_name character varying(2048),
    process_code character varying(64),
    process_name character varying(2048),
    process_leader_code character varying(64),
    process_leader_name character varying(2048),
    process_feedback text,
    process_leader_instruction text,
    cancel_reason character varying(256),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone,
	PJLYH character varying(256)
);
ALTER TABLE ONLY icc_t_alarm_process ADD CONSTRAINT icc_t_alarm_process_pkey PRIMARY KEY (id); 
CREATE INDEX idx_t_alarm_process_threeinone ON icc_t_alarm_process USING btree (alarm_id, time_signed, state);
CREATE INDEX idx_t_alarm_process_threeinone2 ON icc_t_alarm_process USING btree (process_dept_code, time_signed, state);
CREATE INDEX idx_t_alarm_process_time_submit ON icc_t_alarm_process USING brin (time_submit) WITH (pages_per_range=1);
create index icc_t_alarm_process_crttime on icc_t_alarm_process(create_time);
create index icc_t_alarm_process_uptime on icc_t_alarm_process(update_time);
DROP TABLE IF EXISTS "public"."icc_t_alarm_process_book";
CREATE TABLE icc_t_alarm_process_book (
    guid character varying(64) NOT NULL,
    id character varying(64) NOT NULL,
    alarm_id character varying(64),
    state character varying(64),
    time_edit timestamp(6) without time zone,
    time_submit timestamp(6) without time zone,
    time_arrived timestamp(6) without time zone,
    time_signed timestamp(6) without time zone,
    time_feedback timestamp(6) without time zone,
    is_need_feedback character varying(64),
    dispatch_dept_district_code character varying(64),
    dispatch_dept_code character varying(64),
    dispatch_dept_name character varying(2048),
    dispatch_code character varying(64),
    dispatch_name character varying(2048),
    dispatch_leader_code character varying(64),
    dispatch_leader_name character varying(2048),
    dispatch_suggestion text,
    dispatch_leader_instruction text,
    process_dept_district_code character varying(64),
    process_dept_code character varying(64),
    process_dept_name character varying(2048),
    process_code character varying(64),
    process_name character varying(2048),
    process_leader_code character varying(64),
    process_leader_name character varying(2048),
    process_feedback text,
    process_leader_instruction text,
    flag character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_alarm_process_book ADD CONSTRAINT icc_t_alarm_process_book_pkey PRIMARY KEY (guid,flag);
CREATE INDEX idx_t_alarm_process_book_threeinone ON icc_t_alarm_process_book USING btree (alarm_id, time_signed, state);
CREATE INDEX idx_t_alarm_process_book_time_submit ON icc_t_alarm_process_book USING brin (time_submit) WITH (pages_per_range=1);

DROP TABLE IF EXISTS "public"."icc_t_pjdb_book";
CREATE TABLE icc_t_pjdb_book (
    guid character varying(64) NOT NULL,
    flag character varying(64),
	xzqhdm character varying(12), 
    pjdbh character varying(30), 
    jjdbh character varying(30), 
	pjdwdm character varying(12),
	pjybh character varying(30), 
	pjyxm character varying(50),
	pjlyh character varying(255),
	pjyj text,
	cjdwdm character varying(12), 
	pjsj timestamp(6) without time zone,
    xtzdddsj timestamp(6) without time zone,
    pdqssj timestamp(6) without time zone, 
	qsryxm character varying(50), 
	qsrym character varying(30),
	cdry character varying(200), 
	cdcl character varying(500), 
	cdct character varying(500), 
	jqclztdm character varying(64),
	cjsj timestamp(6) without time zone,
	gxsj timestamp(6) without time zone,
	pjdwmc character varying(2048),
    pjdwdmbs character varying(64),
	cjdwmc character varying(2048), 
	cjdwdmbs character varying(64), 
	cjdxlx character varying(64),
	cjdxmc character varying(64),
	cjdxid character varying(64),
    dqywzt character varying(64),
	zxbh character varying(64), 
	tdsj timestamp(6) without time zone,
	qxyy character varying(256),
	sfzddpcj character varying(64),
	cjry character varying(50),
    xgry character varying(50),
	cszt character varying(32)
);
ALTER TABLE ONLY icc_t_pjdb_book ADD CONSTRAINT icc_t_pjdb_book_pkey PRIMARY KEY (guid,flag);
CREATE INDEX idx_t_pjdb_book_threeinone ON icc_t_pjdb_book USING btree (jjdbh, pdqssj, jqclztdm);
CREATE INDEX idx_t_pjdb_book_time_submit ON icc_t_pjdb_book USING brin (pjsj) WITH (pages_per_range=1);

DROP TABLE IF EXISTS "public"."icc_t_alarm_remark";
CREATE TABLE icc_t_alarm_remark (
    guid character varying(64) NOT NULL,
    alarm_id character varying(64) NOT NULL,
    feedback_code character varying(64),
    feedback_name character varying(2048),
    feedback_dept_code character varying(64),
    feedback_dept_name character varying(2048),
    feedback_time timestamp(6) without time zone,
    content text,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_alarm_remark ADD CONSTRAINT icc_t_alarm_remark_pkey PRIMARY KEY (guid);

DROP TABLE IF EXISTS "public"."icc_t_alarm_transport";
CREATE TABLE icc_t_alarm_transport (
    guid character varying(64) NOT NULL,
    source_alarm_id character varying(64) NOT NULL,
    source_callref_id character varying(64),
    source_user_code character varying(64),
    source_user_name character varying(2048),
    source_seat_no character varying(64),
    source_dept_code character varying(64),
    source_dept_name character varying(2048),
    target_user_code character varying(64),
    target_user_name character varying(2048),
    target_seat_no character varying(64),
    target_dept_code character varying(64),
    target_dept_name character varying(2048),
    target_alarm_id character varying(64),
    target_callref_id character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_alarm_transport ADD CONSTRAINT icc_t_alarm_transport_pkey PRIMARY KEY (guid);

DROP TABLE IF EXISTS "public"."icc_t_alarm_visit";
CREATE TABLE icc_t_alarm_visit (
  id character varying(64) NOT NULL,
  alarm_id character varying(64) NOT NULL,
  callref_id character varying(64) NOT NULL,
  "time" timestamp(6) without time zone NOT NULL,
  dept_code character varying(64),
  dept_name character varying(2048),
  caller_name character varying(2048),
  visitor_code character varying(64),
  visitor_name character varying(2048),
  caller_no character varying(64),
  satisfaction character varying(64),
  suggest text,
  result_content text,
  call_type character varying(64),
  reason text
);
ALTER TABLE icc_t_alarm_visit ADD CONSTRAINT icc_t_alarm_visit_pkey PRIMARY KEY (id);

DROP TABLE IF EXISTS "public"."icc_t_bind";
CREATE TABLE icc_t_bind (
    guid character varying(128) NOT NULL,
    type character varying(64),
    from_guid character varying(64),
    to_guid character varying(64),
    is_delete character varying(64),
    level integer,
    sort integer,
    shortcut character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_bind ADD CONSTRAINT icc_t_bind_pkey PRIMARY KEY (guid);

DROP TABLE IF EXISTS "public"."icc_t_blacklist";
CREATE TABLE icc_t_blacklist (
    guid character varying(64) NOT NULL,
	limit_type character varying(32),
    limit_num character varying(64),
    limit_minute character varying(64),
    limit_reason character varying(128),
    staff_code character varying(64),
    staff_name character varying(2048),
    is_delete character varying(64),
    begin_time timestamp(6) without time zone,
    end_time timestamp(6) without time zone,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone,
	approver character varying(64),
	approved_info character varying(64),
	receipt_dept_name character varying(64),
	receipt_dept_district_code character varying(64)
);
ALTER TABLE ONLY icc_t_blacklist ADD CONSTRAINT icc_t_blacklist_pkey PRIMARY KEY (guid);

DROP TABLE IF EXISTS "public"."icc_t_redlist";
CREATE TABLE icc_t_redlist (
	id character varying(64) NOT NULL,
    phone_num character varying(32),
    name character varying(2048),
    department character varying(256),
    position character varying(256),
	remark text,
    is_delete character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_redlist ADD CONSTRAINT icc_t_redlist_pkey PRIMARY KEY (id);

DROP TABLE IF EXISTS "public"."icc_t_backalarm";
CREATE TABLE icc_t_backalarm (
    guid character varying(64) NOT NULL,
    process_id character varying(64) NOT NULL,    
    back_reason text     
);
ALTER TABLE ONLY icc_t_backalarm ADD CONSTRAINT icc_t_backalarm_pkey PRIMARY KEY (guid);
CREATE INDEX icc_t_backalarm_process_id ON icc_t_backalarm USING btree (process_id);
DROP TABLE IF EXISTS "public"."icc_t_callback_relation";
CREATE TABLE icc_t_callback_relation (
    callref_id character varying(64) NOT NULL,
    alarm_id character varying(64),
    receipt_code character varying(64),
    receipt_name character varying(2048),
    "time" timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_callback_relation ADD CONSTRAINT icc_t_callback_relation_pkey PRIMARY KEY (callref_id);

DROP TABLE IF EXISTS "public"."icc_t_can_admin_dept";
CREATE TABLE icc_t_can_admin_dept (
    guid character varying(64) NOT NULL,
    parent_guid character varying(64),
    bind_dept_code character varying(64),
    bind_parent_dept_code character varying(64),
    belong_dept_code character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_can_admin_dept ADD CONSTRAINT icc_t_admin_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_can_dispatch_dept";
CREATE TABLE icc_t_can_dispatch_dept (
    guid character varying NOT NULL,
    parent_guid character varying,
    bind_dept_code character varying,
    bind_parent_dept_code character varying,
    belong_dept_code character varying,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_can_dispatch_dept ADD CONSTRAINT icc_t_can_dispatch_dept_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_client_in_out";
CREATE TABLE icc_t_client_in_out (
    guid character varying(64) NOT NULL,
    client_id character varying(64) NOT NULL,
    client_name character varying(2048),
    login_time timestamp(6) without time zone,
    logout_time timestamp(6) without time zone,
    duration character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_client_in_out ADD CONSTRAINT icc_t_client_in_out_pkey PRIMARY KEY (guid);
ALTER TABLE ONLY icc_t_client_in_out ADD CONSTRAINT unique_client_id_login_time UNIQUE (client_id, login_time);
CREATE INDEX icc_t_client_in_out_create_time ON icc_t_client_in_out USING brin (create_time) WITH (pages_per_range=1);


DROP TABLE IF EXISTS "public"."icc_t_client_on_off";
CREATE TABLE icc_t_client_on_off (
    guid character varying NOT NULL,
    client_id character varying(64) NOT NULL,
    client_name character varying(2048),
    on_time timestamp(6) without time zone,
    off_time timestamp(6) without time zone,
    duration character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone,
    dept_code character varying(64),
	receipt_code character varying(64) NOT NULL,
    receipt_name character varying(2048)
);
ALTER TABLE ONLY icc_t_client_on_off ADD CONSTRAINT icc_t_client_on_off_pkey PRIMARY KEY (guid);
CREATE INDEX idx_t_client_client_id ON icc_t_client_on_off USING btree (client_id);
CREATE INDEX idx_t_client_onoff_time ON icc_t_client_on_off USING brin  (off_time, on_time) WITH (pages_per_range=1);
CREATE INDEX index_onoff_on_time ON icc_t_client_on_off USING brin (on_time) WITH (pages_per_range=1);
DROP TABLE IF EXISTS "public"."icc_t_config";
CREATE TABLE icc_t_config (
    ip character varying NOT NULL,
    config text,
    is_delete character varying(64),
    level integer,
    sort integer,
    shortcut character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_config ADD CONSTRAINT icc_t_config_pkey PRIMARY KEY (ip);
DROP TABLE IF EXISTS "public"."icc_t_dept";
CREATE TABLE icc_t_dept (
    guid character varying(64) NOT NULL,
    parent_guid character varying(64),
    code character varying(64),
    district_code character varying(64),
    type character varying(64),
    name character varying(2048),
	path character varying(512),
	depth character varying(64),
	pucorgidentifier character varying(512),
    phone character varying(64),
    is_delete character varying(64),
    level integer,
    sort integer,
    shortcut character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_dept ADD CONSTRAINT icc_t_dept_pkey PRIMARY KEY (guid);
CREATE INDEX idx_t_icc_code ON icc_t_dept USING gin (code gin_trgm_ops);
CREATE INDEX idx_t_dept_phone ON icc_t_dept USING btree (phone);
CREATE INDEX icc_t_dept_path on icc_t_dept using GIST (path gist_trgm_ops);
DROP TABLE IF EXISTS "public"."icc_t_dict";
CREATE TABLE icc_t_dict (
    guid character varying NOT NULL,
    parent_guid character varying(64),
    code character varying(64),
    is_delete character varying(64),
    level integer,
    sort integer,
    shortcut character varying(64),
    create_user character varying(256),
    create_time timestamp(6) without time zone,
    update_user character varying(256),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_dict ADD CONSTRAINT icc_t_dict_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_dict_value";
CREATE TABLE icc_t_dict_value (
    guid character varying NOT NULL,
	dict_key character varying(64),
	parent_key character varying(64),	
	dict_type character varying(64),
	dict_code character varying(64),
    value character varying(256),
    dict_guid character varying(64),
    lang_code character varying(64),
    is_delete character varying(64),
    level integer,
    sort integer,
    shortcut character varying(64),
    create_user character varying(256),
    create_time timestamp(6) without time zone,
    update_user character varying(256),
    update_time timestamp(6) without time zone,
	union_key	character varying(64),
	parentUnion_key	character varying(64),
	dictValue_json character varying(512)
);
ALTER TABLE ONLY icc_t_dict_value ADD CONSTRAINT icc_t_dict_value_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_dispatch";
CREATE TABLE icc_t_dispatch (
    guid character varying(64) NOT NULL,
    parent_guid character varying(64),
    code character varying(64),
    district_code character varying(64),
    type character varying(64),
    name character varying(2048),
    phone character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_dispatch ADD CONSTRAINT icc_t_dispatch_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_func";
CREATE TABLE icc_t_func (
    guid character varying(64) NOT NULL,
    parent_guid character varying(64),
    code character varying(64),
    name character varying(2048),
    is_delete character varying(64),
    level integer,
    sort integer,
    shortcut character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_func ADD CONSTRAINT icc_t_func_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_gps_record";
CREATE TABLE icc_t_gps_record (
    callref_id character varying(64) NOT NULL,
    type character varying(64),
    phone_no character varying(64),
    longitude double precision,
    latitude double precision,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_gps_record ADD CONSTRAINT icc_t_gps_record_pkey PRIMARY KEY (phone_no);
DROP TABLE IF EXISTS "public"."icc_t_keydept";
CREATE TABLE icc_t_keydept (
    guid character varying NOT NULL,
    type character varying(64),
    name character varying(2048),
    phone character varying(64),
    address text,
    longitude character varying,
    latitude character varying,
    is_delete character varying,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_keydept ADD CONSTRAINT icc_t_keydept_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_language";
CREATE TABLE icc_t_language (
    guid character varying(64) NOT NULL,
    code character varying(64),
    name character varying(2048),
    is_delete character varying(256),
    level integer,
    sort integer,
    shortcut character varying(64),
    create_user character varying(256),
    create_time timestamp(6) without time zone,
    update_user character varying(256),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_language ADD CONSTRAINT icc_t_language_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_param";
CREATE TABLE icc_t_param (
    name character varying(64) NOT NULL,
    value text,
    is_visibility character varying(5),
    is_readonly character varying(5),
	description character varying(2048),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_param ADD CONSTRAINT icc_t_param_pkey PRIMARY KEY (name);
DROP TABLE IF EXISTS "public"."icc_t_phone_division";
CREATE TABLE icc_t_phone_division (
    phone_pre character varying(64) NOT NULL,
    province character varying(64),
    city character varying(2048),
    operator character varying(64),
	out_call_head character varying(64),
    remark character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_phone_division ADD CONSTRAINT icc_t_phone_division_pkey PRIMARY KEY (phone_pre);
DROP TABLE IF EXISTS "public"."icc_t_phoneinfo";
CREATE TABLE icc_t_phoneinfo (
    guid character varying(64) NOT NULL,
    phone character varying(64) NOT NULL,
    name character varying(2048),
    address text,
    is_delete character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_phoneinfo ADD CONSTRAINT icc_t_phoneinfo_pkey PRIMARY KEY (guid);
ALTER TABLE ONLY icc_t_phoneinfo ADD CONSTRAINT icc_t_phoneinfo_guid_phone_key UNIQUE (guid, phone);
CREATE INDEX idx_t_phoneinfo_phone ON icc_t_phoneinfo USING btree (phone);
DROP TABLE IF EXISTS "public"."icc_t_phonemark";
CREATE TABLE icc_t_phonemark (
    guid character varying NOT NULL,
    phone character varying(64),
    type character varying(64),
    staff_code character varying(64),
    remark text,
    is_delete character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_phonemark ADD CONSTRAINT icc_t_phonemark_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_role";
CREATE TABLE icc_t_role (
    guid character varying(64) NOT NULL,
    parent_guid character varying(64),
    code character varying(64),
    name character varying(2048),
	org_guid character varying(64),
    is_delete character varying(64),
    level integer,
    sort integer,
    shortcut character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_role ADD CONSTRAINT icc_t_role_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_seat";
CREATE TABLE icc_t_seat (
    guid character varying(64) NOT NULL,
    no character varying(64) NOT NULL,
    name character varying(2048),
    dept_code character varying(64),
    dept_name character varying(2048),
    is_delete character varying(64),
    sort integer,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_seat ADD CONSTRAINT icc_t_seat_pkey PRIMARY KEY (guid, no);
CREATE INDEX idx_t_seat_isdelete_deptcode ON icc_t_seat USING btree (is_delete, dept_code);
DROP TABLE IF EXISTS "public"."icc_t_shift";
CREATE TABLE icc_t_shift (
    guid character varying NOT NULL,
    start_time timestamp(6) without time zone,
    end_time timestamp(6) without time zone,
    dept_code character varying(64) NOT NULL,
	dept_code_path character varying(2048),
    duty_code character varying(64),
    duty_name character varying(2048),
    duty_leader_code character varying(64),
    duty_leader_name character varying(2048),
	receiver_code character varying(64),
    receiver_name character varying(2048),
	receive_state character varying(64),
    work_content text,
    important_content text,
    shift_content text,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_shift ADD CONSTRAINT icc_t_shift_pkey PRIMARY KEY (guid);
CREATE INDEX idx_icc_t_shift_dept_code_path ON icc_t_shift USING btree (dept_code_path);
DROP TABLE IF EXISTS "public"."icc_t_staff";
CREATE TABLE icc_t_staff (
    guid character varying(64) NOT NULL,
    dept_guid character varying(64),
    code character varying(64),
    name character varying(2048),
    sex character varying(64),
    type character varying(64),
    post character varying(1024),
    mobile character varying(64),
    phone character varying(64),
    is_delete character varying(64),
    level integer,
    sort integer,
    shortcut character varying(64),
	isleader character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone,
	staff_id_no character varying(64)
);
ALTER TABLE ONLY icc_t_staff ADD CONSTRAINT icc_t_staff_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_statistic";
CREATE TABLE icc_t_statistic (
    guid character varying(64) NOT NULL,
    client_name character varying(2048) NOT NULL,
    client_id character varying(64),
    dept_name character varying(2048),
    online_time timestamp(6) without time zone NOT NULL,
    offline_time timestamp(6) without time zone,
    online_time_len integer,
    offseat_time_len integer,
    release_call_num integer,
    total_call_num integer,
    alarm_num integer
);
ALTER TABLE ONLY icc_t_statistic ADD CONSTRAINT icc_t_statistic_pkey PRIMARY KEY (guid);

DROP TABLE IF EXISTS "public"."icc_t_text_template";
CREATE TABLE icc_t_text_template (
    guid character varying NOT NULL,
    type character varying(64),
    content text,
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone,
	alarm_type character varying(64),
	handly_type character varying(64)
);
ALTER TABLE ONLY icc_t_text_template ADD CONSTRAINT icc_t_text_template_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_third_alarm";
CREATE TABLE icc_t_third_alarm (
    guid character varying(64) NOT NULL,
    type character varying(64),
    address text,
    content text,
    is_phone character varying(16),
    create_date timestamp(6) without time zone,
    user_code character varying(64),
    user_name character varying(2048),
    user_phone character varying(64),
    user_address text,
    unit_contactor character varying(2048),
    hand_phone character varying(64),
    to_org_id character varying(64),
    to_org_name character varying(2048),
    to_user_id character varying(64),
    to_user_name character varying(2048),
    state character varying(16),
    com_no character varying(64),
    open_id character varying(64),
    long character varying(64),
    lat character varying(64),
    remark1 text,
    remark2 text
);
ALTER TABLE ONLY icc_t_third_alarm ADD CONSTRAINT icc_t_third_alarm_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_third_changeinfo";
CREATE TABLE icc_t_third_changeinfo (
    guid character varying(64) NOT NULL,
    third_alarm_guid character varying(64),
    orientation character varying(16),
    state character varying(16),
    phone character varying(64),
    open_id character varying(64),
    change_type character varying(64),
    change_content text,
    change_date timestamp(6) without time zone,
    long character varying(64),
    lat character varying(64),
    remark1 text,
    remark2 text,
    fguid character varying(64),
    fname character varying(2048)
);
ALTER TABLE ONLY icc_t_third_changeinfo ADD CONSTRAINT icc_t_third_changeinfo_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_user";
CREATE TABLE icc_t_user (
    guid character varying(64) NOT NULL,
    code character varying(64),
    name character varying(2048),
    pwd character varying(2048),
    is_delete character varying(64),
    level integer,
    sort integer,
    shortcut character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone,
	user_type character varying(64),
	staff_guid character varying(64),
	org_guid character varying(64),
    remark text
);
ALTER TABLE ONLY icc_t_user ADD CONSTRAINT icc_t_user_pkey PRIMARY KEY (guid);
ALTER TABLE ONLY icc_t_user ADD CONSTRAINT user_code UNIQUE (code);
DROP TABLE IF EXISTS "public"."icc_t_voicerecord";
CREATE TABLE icc_t_voicerecord (
    callref_id character varying(64) NOT NULL,
    caller_id character varying(64),
    called_id character varying(64),
    record_file character varying(64),
    file_path character varying(255),
    begin_time timestamp(6) without time zone,
    end_time timestamp(6) without time zone,
    duration character varying(64)
);
ALTER TABLE ONLY icc_t_voicerecord ADD CONSTRAINT icc_t_voicerecord_pkey PRIMARY KEY (callref_id);
DROP TABLE IF EXISTS "public"."icc_t_notice";
CREATE TABLE icc_t_notice (
    notice_id character varying(64) NOT NULL,
    user_code character varying(64),
	user_name character varying(2048),
	seat_no character varying(64),
	dept_code character varying(64),
	dept_name character varying(2048),
	target_dept_code character varying(64),
	target_dept_name character varying(2048),
	notice_type character varying(16),
	title character varying(255),
	content text,
	"time" timestamp(6) without time zone,
	is_delete character varying(16),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_notice ADD CONSTRAINT icc_t_notice_pkey PRIMARY KEY (notice_id);
DROP TABLE IF EXISTS "public"."icc_t_tips";
CREATE TABLE icc_t_tips (
    tips_id character varying(64) NOT NULL,
    called_no_type character varying(64),
	level character varying(64),
	first_type character varying(64),
	tips_content text,
	is_delete character varying(16),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_tips ADD CONSTRAINT icc_t_tips_pkey PRIMARY KEY (tips_id);
DROP TABLE IF EXISTS "public"."icc_t_timeout_event";
CREATE TABLE icc_t_timeout_event (
  guid character varying(64) NOT NULL,
  alarm_id character varying(64),
  callref_id character varying(64),
  receipt_dept_code character varying(64),
  receipt_dept_name character varying(2048),
  receipt_code character varying(64),
  receipt_name character varying(2048),
  receipt_seatno character varying(64),
  start_time timestamp(6) without time zone,
  end_time timestamp(6) without time zone,
  alarm_type character varying(32),
  is_delete character varying(16),
  create_user character varying(64),
  create_time timestamp(6) without time zone,
  update_user character varying(64),
  update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_timeout_event ADD CONSTRAINT icc_t_timeout_event_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_alarm_statistics";
CREATE TABLE icc_t_alarm_statistics(
    statistics_id character varying(128) NOT NULL,
    statistics_type character varying(64) NOT NULL,
    statistics_date character varying(64) NOT NULL,
	statistics_result character varying(64) NOT NULL,
    dept_code character varying(64),
    dept_name character varying(2048),
	statistics_time character varying(64),
    remark character varying(128)   
);
ALTER TABLE ONLY icc_t_alarm_statistics ADD CONSTRAINT icc_t_alarm_statistics_pkey PRIMARY KEY (statistics_id);
DROP TABLE IF EXISTS "public"."icc_t_chat_group";
CREATE TABLE icc_t_chat_group (
  group_id varchar(64) NOT NULL,
  group_name varchar(2048),
  admin_code varchar(64),
  is_delete varchar(8)
);
ALTER TABLE icc_t_chat_group ADD CONSTRAINT icc_t_chat_group_pkey PRIMARY KEY (group_id);
DROP TABLE IF EXISTS "public"."icc_t_chat_group_members";
CREATE TABLE icc_t_chat_group_members (
  group_id varchar(64) NOT NULL,
  staff_code varchar(64) NOT NULL,
  staff_name varchar(2048)
);
ALTER TABLE icc_t_chat_group_members ADD CONSTRAINT chat_group_members_pkey PRIMARY KEY (group_id, staff_code);
CREATE INDEX chat_group_members_group_id ON icc_t_chat_group_members USING btree (group_id);

DROP TABLE IF EXISTS "public"."icc_t_chat_records";
CREATE TABLE icc_t_chat_records (
  "msg_id" varchar(64) NOT NULL,
  "sender" varchar(64),
  "seat_no" varchar(64),
  "receiver" varchar(64),
  "time" timestamp(6),
  "is_group_msg" varchar(8),
  "content" text,
  "type" varchar(8)
);
ALTER TABLE icc_t_chat_records ADD CONSTRAINT chat_records_pkey PRIMARY KEY (msg_id);

CREATE INDEX icc_chat_records_receiver ON icc_t_chat_records USING btree (receiver);
CREATE INDEX icc_chat_records_sender ON icc_t_chat_records USING btree (sender);
CREATE INDEX icc_chat_records_time ON icc_t_chat_records USING brin (time) WITH (pages_per_range=1);
DROP TABLE IF EXISTS "public"."icc_t_chat_records_tag";
CREATE TABLE icc_t_chat_records_tag (
  msg_id varchar(64) NOT NULL,
  receiver varchar(64) NOT NULL,
  is_read varchar(8)
);
ALTER TABLE icc_t_chat_records_tag ADD CONSTRAINT chat_records_tag_pkey PRIMARY KEY (msg_id, receiver);
CREATE INDEX idx_records_tag_isread ON icc_t_chat_records_tag USING btree (is_read);
CREATE INDEX idx_records_tag_receiver ON icc_t_chat_records_tag USING btree (receiver);
DROP TABLE IF EXISTS "public"."icc_t_addressbook_org";
CREATE TABLE icc_t_addressbook_org (
    guid character varying(64) NOT NULL,
    parent_guid character varying(64),
    code character varying(64),
    name character varying(2048),
    phone character varying(64),
	remark text,
    sort integer,
	is_delete character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_addressbook_org ADD CONSTRAINT icc_t_addressbook_org_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_addressbook_phone";
CREATE TABLE icc_t_addressbook_phone (
    guid character varying(64) NOT NULL,
    org_guid character varying(64),
    code character varying(64),
    name character varying(2048),
    gender character varying(64),
    mobile character varying(64),
    phone character varying(64),
    remark text,
    sort integer,
	is_delete character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_addressbook_phone ADD CONSTRAINT icc_t_addressbook_phone_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_addressbook_group";
CREATE TABLE icc_t_addressbook_group (
    guid character varying(64) NOT NULL,
    title character varying(512),
    user_code character varying(64),
    type character varying(64),
    sort integer,
	is_delete character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_addressbook_group ADD CONSTRAINT icc_t_addressbook_group_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_addressbook_contact";
CREATE TABLE icc_t_addressbook_contact (
    guid character varying(64) NOT NULL,
    group_guid character varying(64),
    type character varying(64),
	name character varying(2048),
	mobile character varying(64),
	phone character varying(64),
	sort character varying(32),
    remark text,
	is_delete character varying(64),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_addressbook_contact ADD CONSTRAINT icc_t_addressbook_contact_pkey PRIMARY KEY (guid);
DROP TABLE IF EXISTS "public"."icc_t_violation";
CREATE TABLE icc_t_violation (
    guid character varying(64) NOT NULL,
    violation_id character varying(64),
    violation_type character varying(64),	
	violation_target character varying(64),
    violation_user character varying(256),
	violation_time timestamp(6) without time zone,
	timeout_value integer,
	dept_code character varying(256),
    create_time timestamp(6) without time zone,
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_violation ADD CONSTRAINT icc_t_violation_pkey PRIMARY KEY (guid);
CREATE INDEX idx_t_violation_violation_time ON icc_t_violation USING brin (violation_time) WITH (pages_per_range='1');
DROP TABLE IF EXISTS "public"."icc_t_keyword";
CREATE TABLE icc_t_keyword (
    guid character varying(64) NOT NULL,    
    key_word character varying(256) NOT NULL,
    content text,	    
    create_user character varying(256),
    create_time timestamp(6) without time zone,
    update_user character varying(256),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_keyword ADD CONSTRAINT icc_t_keyword_pkey PRIMARY KEY (guid);
ALTER TABLE ONLY icc_t_keyword ADD CONSTRAINT unique_icc_t_keyword UNIQUE (key_word);
DROP TABLE IF EXISTS "public"."icc_t_bind_ip_seat";
CREATE TABLE icc_t_bind_ip_seat (
    guid character varying(64) NOT NULL,
	ip character varying(64),
	seat_no character varying(32),
	is_delete character varying(32),
    create_user character varying(64),
    create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone
);
ALTER TABLE ONLY icc_t_bind_ip_seat ADD CONSTRAINT icc_t_bind_ip_seat_pkey PRIMARY KEY (guid);

DROP TABLE IF EXISTS "public"."icc_t_dict_code_map";
CREATE TABLE icc_t_dict_code_map (
  id character varying(100),
  dict_code character varying(30) NOT NULL,
  peer_code character varying(30),
  type character varying(30),
  level character varying(64),
  direction character varying(10)
);
COMMENT ON COLUMN icc_t_dict_code_map.dict_code IS '本地字典编码';
COMMENT ON COLUMN icc_t_dict_code_map.peer_code IS '外部字典编码';
COMMENT ON COLUMN icc_t_dict_code_map.type IS '类型';
COMMENT ON COLUMN icc_t_dict_code_map.level IS '级别';
COMMENT ON COLUMN icc_t_dict_code_map.direction IS '方向';
DROP TABLE IF EXISTS "public"."icc_t_ds_dict_case_type";
CREATE TABLE icc_t_ds_dict_case_type (
  dm character varying(10),
  bh character varying(20),
  jz character varying(10),
  mc character varying(80),
  qc character varying(400),
  sjbh character varying(20),
  yxbz character varying(10),
  sjc date
);
COMMENT ON COLUMN icc_t_ds_dict_case_type.dm IS '代码';
COMMENT ON COLUMN icc_t_ds_dict_case_type.bh IS '编号';
COMMENT ON COLUMN icc_t_ds_dict_case_type.jz IS '警钟';
COMMENT ON COLUMN icc_t_ds_dict_case_type.mc IS '名称';
COMMENT ON COLUMN icc_t_ds_dict_case_type.qc IS '全称';
COMMENT ON COLUMN icc_t_ds_dict_case_type.sjbh IS '上级编号';
COMMENT ON COLUMN icc_t_ds_dict_case_type.yxbz IS '有效标记';
COMMENT ON COLUMN icc_t_ds_dict_case_type.sjc IS '时间戳';
DROP TABLE IF EXISTS "public"."icc_t_ds_dict_source_type";
CREATE TABLE icc_t_ds_dict_source_type (
  bh character varying(10)  NOT NULL,
  dm character varying(20),
  mc character varying(80),
  yxbz character varying(10),
  sjc date
);
COMMENT ON COLUMN icc_t_ds_dict_source_type.bh IS '编码';
COMMENT ON COLUMN icc_t_ds_dict_source_type.dm IS '代码';
COMMENT ON COLUMN icc_t_ds_dict_source_type.mc IS '名称';
COMMENT ON COLUMN icc_t_ds_dict_source_type.yxbz IS '有效标记';
DROP TABLE IF EXISTS "public"."icc_t_post_data_message";
CREATE TABLE icc_t_post_data_message (
  id character varying(50) NOT NULL,
  datakey character varying(255),
  type character varying(255),
  posttime date,
  status character varying(255),
  errcode character varying(255),
  errmessage character varying(255) 
);
COMMENT ON COLUMN icc_t_post_data_message.id IS 'ID';
COMMENT ON COLUMN icc_t_post_data_message.datakey IS '关键字';
COMMENT ON COLUMN icc_t_post_data_message.type IS '类型';
COMMENT ON COLUMN icc_t_post_data_message.posttime IS '上传时间';
COMMENT ON COLUMN icc_t_post_data_message.status IS '上传状态';
COMMENT ON COLUMN icc_t_post_data_message.errcode IS '错误代码';
COMMENT ON COLUMN icc_t_post_data_message.errmessage IS '错误信息';

DROP TABLE IF EXISTS "public"."icc_t_alarm_monitor_seat_approval";
CREATE TABLE icc_t_alarm_monitor_seat_approval (
    guid character varying(64) NOT NULL,
    apply_type character varying(64) NOT NULL,
    apply_target_id character varying(64) NOT NULL,
    apply_time timestamp(6) without time zone,
    apply_body text,
    apply_staff_code character varying(64),
	apply_staff_name character varying(64),
	apply_org_code character varying(64),
    apply_org_name character varying(2048),
    apply_remark text,
    approve_staff_code character varying(64),
    approve_staff_name character varying(64),
	approve_result character varying(64),
    approve_time timestamp(6) without time zone,
 	approve_remark text
);
ALTER TABLE ONLY icc_t_alarm_monitor_seat_approval ADD CONSTRAINT icc_t_alarm_monitor_seat_approval_pkey PRIMARY KEY (guid,approve_result);
CREATE INDEX idx_t_alarm_monitor_seat_approval_id ON icc_t_alarm_monitor_seat_approval USING btree (apply_target_id);
CREATE INDEX idx_t_alarm_monitor_seat_approval_apply_result ON icc_t_alarm_monitor_seat_approval USING btree (approve_result);
CREATE INDEX idx_t_alarm_monitor_seat_approval_apply_time ON icc_t_alarm_monitor_seat_approval USING brin (apply_time) WITH (pages_per_range='1');


DROP TABLE IF EXISTS "public"."icc_t_keyword_alarm";
CREATE TABLE icc_t_keyword_alarm (guid character varying(64) NOT NULL,  
 alarm_id character varying(64) NOT NULL,
 key_word character varying(256) NOT NULL,
 key_word_content text,
 content text,
 state character varying(64),
 dept_code character varying(256),
 create_user character varying(64),
 create_time timestamp(6) without time zone);
ALTER TABLE ONLY icc_t_keyword_alarm ADD CONSTRAINT icc_t_keyword_alarm_pkey PRIMARY KEY (guid);
CREATE INDEX idx_icc_t_keyword_alarm_alarmid ON icc_t_keyword_alarm USING brin (alarm_id);
CREATE INDEX idx_icc_t_keyword_alarm_createtime ON icc_t_keyword_alarm USING brin (create_time) WITH (pages_per_range='1');
DROP TABLE IF EXISTS "public"."icc_t_seat_address";
CREATE TABLE icc_t_seat_address (guid character varying(64) NOT NULL,  
 seat_no character varying(64) NOT NULL,
 address text,
 create_time timestamp(6) without time zone,
 update_time timestamp(6) without time zone);
 ALTER TABLE ONLY icc_t_seat_address ADD CONSTRAINT icc_t_seat_address_pkey PRIMARY KEY (guid);
 ALTER TABLE ONLY icc_t_seat_address ADD CONSTRAINT unique_t_seat_address UNIQUE (seat_no);


DROP TABLE IF EXISTS icc_t_plan_base;
CREATE TABLE icc_t_plan_base(
                                id VARCHAR(64) NOT NULL,
                                name VARCHAR(150),
                                key_word VARCHAR(255),
                                alarm_type_code TEXT,
                                alarm_type_name TEXT,
                                alarm_level_code TEXT,
                                alarm_level_name TEXT,
                                alarm_label_code TEXT,
                                alarm_label_name TEXT,
                                enable_code VARCHAR(10),
                                enable_name VARCHAR(30),
                                create_user_id VARCHAR(50),
                                create_user_name VARCHAR(150),
                                create_org_code VARCHAR(20),
                                create_org_name VARCHAR(150),
                                create_time TIMESTAMP,
                                update_user_id VARCHAR(50),
                                update_user_name VARCHAR(150),
                                update_org_code VARCHAR(20),
                                update_org_name VARCHAR(150),
                                update_time TIMESTAMP,
                                validity INTEGER,
                                PRIMARY KEY (id)
);

COMMENT ON TABLE icc_t_plan_base IS '预案基础信息表';
COMMENT ON COLUMN icc_t_plan_base.id IS '主键';
COMMENT ON COLUMN icc_t_plan_base.name IS '预案名称';
COMMENT ON COLUMN icc_t_plan_base.key_word IS '关键字';
COMMENT ON COLUMN icc_t_plan_base.alarm_type_code IS '警情类型代码(字典)';
COMMENT ON COLUMN icc_t_plan_base.alarm_type_name IS '警情类型名称';
COMMENT ON COLUMN icc_t_plan_base.alarm_level_code IS '警情级别代码(字典)';
COMMENT ON COLUMN icc_t_plan_base.alarm_level_name IS '警情级别名称';
COMMENT ON COLUMN icc_t_plan_base.alarm_label_code IS '警情标签代码(字典)';
COMMENT ON COLUMN icc_t_plan_base.alarm_label_name IS '警情标签名称';
COMMENT ON COLUMN icc_t_plan_base.enable_code IS '状态代码：01-启用，02-不启用';
COMMENT ON COLUMN icc_t_plan_base.enable_name IS '状态名称';
COMMENT ON COLUMN icc_t_plan_base.create_user_id IS '创建人ID';
COMMENT ON COLUMN icc_t_plan_base.create_user_name IS '创建人名称';
COMMENT ON COLUMN icc_t_plan_base.create_org_code IS '创建单位代码';
COMMENT ON COLUMN icc_t_plan_base.create_org_name IS '创建单位名称';
COMMENT ON COLUMN icc_t_plan_base.create_time IS '创建时间';
COMMENT ON COLUMN icc_t_plan_base.update_user_id IS '修改人ID';
COMMENT ON COLUMN icc_t_plan_base.update_user_name IS '修改人名称';
COMMENT ON COLUMN icc_t_plan_base.update_org_code IS '修改单位代码';
COMMENT ON COLUMN icc_t_plan_base.update_org_name IS '修改单位名称';
COMMENT ON COLUMN icc_t_plan_base.update_time IS '修改时间';
COMMENT ON COLUMN icc_t_plan_base.validity IS '是否有效';
DROP TABLE IF EXISTS icc_t_plan_detail;
CREATE TABLE icc_t_plan_detail(
    id VARCHAR(64) NOT NULL,
    base_id VARCHAR(64),
    type VARCHAR(255),
    title VARCHAR(255),
    data TEXT,
    view_type VARCHAR(20),
    data_type VARCHAR(20),
    data_valid TEXT,
    data_view TEXT,
    sort INTEGER,
    create_user_id VARCHAR(50),
    create_user_name VARCHAR(150),
    create_org_code VARCHAR(20),
    create_org_name VARCHAR(150),
    create_time TIMESTAMP,
    update_user_id VARCHAR(50),
    update_user_name VARCHAR(150),
    update_org_code VARCHAR(20),
    update_org_name VARCHAR(150),
	config_note_prefix VARCHAR(1000),
	config_note_suffix VARCHAR(1000),
    update_time TIMESTAMP,
    validity INTEGER,
    PRIMARY KEY (id)
);

COMMENT ON TABLE icc_t_plan_detail IS '预案模块详情信息表';
COMMENT ON COLUMN icc_t_plan_detail.id IS '主键';
COMMENT ON COLUMN icc_t_plan_detail.base_id IS '基础信息表ID';
COMMENT ON COLUMN icc_t_plan_detail.type IS '动态类型：01-警情提示语';
COMMENT ON COLUMN icc_t_plan_detail.title IS '标题';
COMMENT ON COLUMN icc_t_plan_detail.data IS '数据值（数组）';
COMMENT ON COLUMN icc_t_plan_detail.view_type IS '界面类型：Input,Select等';
COMMENT ON COLUMN icc_t_plan_detail.data_type IS '数据类型：String，Array，Date';
COMMENT ON COLUMN icc_t_plan_detail.data_valid IS '数据校验规则：json字符串';
COMMENT ON COLUMN icc_t_plan_detail.data_view IS '数据展示信息：json字符串';
COMMENT ON COLUMN icc_t_plan_detail.sort IS '排序';
COMMENT ON COLUMN icc_t_plan_detail.create_user_id IS '创建人ID';
COMMENT ON COLUMN icc_t_plan_detail.create_user_name IS '创建人名称';
COMMENT ON COLUMN icc_t_plan_detail.create_org_code IS '创建单位代码';
COMMENT ON COLUMN icc_t_plan_detail.create_org_name IS '创建单位名称';
COMMENT ON COLUMN icc_t_plan_detail.create_time IS '创建时间';
COMMENT ON COLUMN icc_t_plan_detail.update_user_id IS '修改人ID';
COMMENT ON COLUMN icc_t_plan_detail.update_user_name IS '修改人名称';
COMMENT ON COLUMN icc_t_plan_detail.update_org_code IS '修改单位代码';
COMMENT ON COLUMN icc_t_plan_detail.update_org_name IS '修改单位名称';
COMMENT ON COLUMN icc_t_plan_detail.update_time IS '修改时间';
COMMENT ON COLUMN icc_t_plan_detail.validity IS '是否有效';
COMMENT ON COLUMN icc_t_plan_detail.config_note_prefix is '配置语前缀';
COMMENT ON COLUMN icc_t_plan_detail.config_note_suffix is '配置语后缀';

DROP TABLE IF EXISTS icc_t_plan_data;
CREATE TABLE icc_t_plan_data(
    id VARCHAR(64) NOT NULL,
    plan_id VARCHAR(64),
    alarm_id VARCHAR(50),
    alarm_num VARCHAR(50),
    alarm_key_word TEXT,
    alarm_type TEXT,
    alarm_level TEXT,
    plan_data TEXT,
    create_user_id VARCHAR(50),
    create_user_name VARCHAR(150),
    create_org_code VARCHAR(20),
    create_org_name VARCHAR(150),
    create_time TIMESTAMP,
    update_user_id VARCHAR(50),
    update_user_name VARCHAR(150),
    update_org_code VARCHAR(20),
    update_org_name VARCHAR(150),
    update_time TIMESTAMP,
    validity INTEGER,
    PRIMARY KEY (id)
);

COMMENT ON TABLE icc_t_plan_data IS '预案数据信息表';
COMMENT ON COLUMN icc_t_plan_data.id IS '主键';
COMMENT ON COLUMN icc_t_plan_data.plan_id IS '预案ID';
COMMENT ON COLUMN icc_t_plan_data.alarm_id IS '警情id';
COMMENT ON COLUMN icc_t_plan_data.alarm_num IS '警情编号';
COMMENT ON COLUMN icc_t_plan_data.alarm_key_word IS '关键字';
COMMENT ON COLUMN icc_t_plan_data.alarm_type IS '警情类型(key-value形式json字符串)';
COMMENT ON COLUMN icc_t_plan_data.alarm_level IS '警情级别(key-value形式json字符串)';
COMMENT ON COLUMN icc_t_plan_data.plan_data IS '预案数据信息(包含模版信息,填写信息)';
COMMENT ON COLUMN icc_t_plan_data.create_user_id IS '创建人ID';
COMMENT ON COLUMN icc_t_plan_data.create_user_name IS '创建人名称';
COMMENT ON COLUMN icc_t_plan_data.create_org_code IS '创建单位代码';
COMMENT ON COLUMN icc_t_plan_data.create_org_name IS '创建单位名称';
COMMENT ON COLUMN icc_t_plan_data.create_time IS '创建时间';
COMMENT ON COLUMN icc_t_plan_data.update_user_id IS '修改人ID';
COMMENT ON COLUMN icc_t_plan_data.update_user_name IS '修改人名称';
COMMENT ON COLUMN icc_t_plan_data.update_org_code IS '修改单位代码';
COMMENT ON COLUMN icc_t_plan_data.update_org_name IS '修改单位名称';
COMMENT ON COLUMN icc_t_plan_data.update_time IS '修改时间';
COMMENT ON COLUMN icc_t_plan_data.validity IS '是否有效';
DROP TABLE IF EXISTS "public"."icc_t_jqdsrdb";
CREATE TABLE public.icc_t_jqdsrdb
(
	jqdsrdbh character varying(64) NOT NULL,
	jjdbh character varying(64),
	pjdbh character varying(64),
	fkdbh character varying(64),
	jqdsrxm character varying(200),
	xbdm character varying(64),
	zjdm character varying(32),
	zjhm character varying(96),
	sf character varying(32),
	sfzdry character varying(64),
	zdrysx character varying(128),
	hjxzqh character varying(6),
	hjxz character varying(200),
	xzxzqh character varying(6),
	xzxz character varying(200),
	gzdw character varying(128),
	zy character varying(128),
	lxdh character varying(60),
	qtzj character varying(32),
	sswpxx character varying(4000),
	cjsj timestamp(6) without time zone,
	gxsj timestamp(6) without time zone,
	csrq timestamp(6) without time zone,
	szqm character varying(512),
	scbs character varying(64),
	rybq character varying(128),
	cjzdbs character varying(50),
	gxzdbs character varying(50)
);
ALTER TABLE public.icc_t_jqdsrdb
ADD CONSTRAINT icc_t_jqdsrdb_pkey PRIMARY KEY (jqdsrdbh);
CREATE INDEX idx_icc_t_jqdsrdb ON icc_t_jqdsrdb USING btree (jjdbh);
DROP TABLE IF EXISTS "public"."icc_t_jqclb";
CREATE TABLE  public.icc_t_jqclb
(
	jqsaclbh character varying(64),
	jjdbh character varying(64),
	zldm character varying(64),
	czxm character varying(128),
	czdh character varying(50),
	czzjhm character varying(50),
	cph character varying(50),
	sfswhcl smallint DEFAULT 0 NOT NULL,
	cjsj timestamp(6) without time zone,
	gxsj timestamp(6) without time zone,
	scbs smallint DEFAULT 0 NOT NULL,
	cjzdbs character varying(50),
	gxzdbs character varying(50)
);

ALTER TABLE public.icc_t_jqclb
ADD CONSTRAINT icc_t_jqclb_pkey PRIMARY KEY (jqsaclbh);
CREATE INDEX idx_icc_t_jqclb ON icc_t_jqclb USING btree (jjdbh);
 
--鑳屾櫙淇℃伅妯℃嫙琛?
DROP TABLE IF EXISTS "public"."icc_t_sfzh_record";
CREATE TABLE icc_t_sfzh_record (
    phone_no character varying(64) NOT NULL,
    sfzh text
);
ALTER TABLE ONLY icc_t_sfzh_record ADD CONSTRAINT icc_t_sfzh_record_pkey PRIMARY KEY (phone_no);
DROP TABLE IF EXISTS "public"."icc_t_sfxx_record";
CREATE TABLE icc_t_sfxx_record (
    sfzh character varying(64) NOT NULL,
    sfxx text
);
ALTER TABLE ONLY icc_t_sfxx_record ADD CONSTRAINT icc_t_sfxx_record_pkey PRIMARY KEY (sfzh);

DROP TABLE IF EXISTS "public"."icc_t_alarm_bjrsfzxx";
CREATE TABLE icc_t_alarm_bjrsfzxx (
    sjhm character varying(64) NOT NULL,
	xm character varying(64) ,
	zjhm character varying(64) ,
    lys text
);
ALTER TABLE ONLY icc_t_alarm_bjrsfzxx ADD CONSTRAINT icc_t_alarm_bjrsfzxx_pkey PRIMARY KEY (sjhm);
DROP TABLE IF EXISTS "public"."icc_t_alarm_bjrxx";
CREATE TABLE icc_t_alarm_bjrxx (
    sfzhm character varying(64) NOT NULL,
	xm character varying(64) ,
	biaoqian text ,
	zjlx character varying(64) ,
	cph text ,
	rylb character varying(64) ,
	ryxl character varying(64) ,
	xb character varying(32) ,
	csrq character varying(32) ,
	mz character varying(32) ,
	jgssxdm character varying(64) ,
	jggjdqdm character varying(64) ,
	hjdqh character varying(128) ,
	hjdqhdm character varying(64) ,
	hjdz character varying(128) ,
	hjdgajgdm character varying(64) ,
	hjdgajg character varying(128) ,
	xzdgajg character varying(128) ,
	xzdgajgdm character varying(64) ,
	xzdqh character varying(128) ,
	xzdqhdm character varying(64) ,
	xzddz character varying(128) ,
	sjhm text ,
	hujibiangeng text ,
    zp text ,
    bzxx text
);
ALTER TABLE ONLY icc_t_alarm_bjrxx ADD CONSTRAINT icc_t_alarm_bjrxx_pkey PRIMARY KEY (sfzhm);
DROP TABLE IF EXISTS "public"."icc_t_alarm_attention";
CREATE TABLE icc_t_alarm_attention (   
	alarm_id character varying(64) NOT NULL,
	attention_staff character varying(64) NOT NULL,
	attention_time timestamp(6) without time zone,
	is_update integer,	
	update_time timestamp(6) without time zone,
	terminal_time timestamp(6) without time zone,
	expire_time timestamp(6) without time zone	
	);
ALTER TABLE ONLY icc_t_alarm_attention ADD CONSTRAINT icc_t_alarm_attention_pkey PRIMARY KEY (alarm_id,attention_staff);

DROP TABLE IF EXISTS "public"."icc_t_frequent_contacts";
CREATE TABLE icc_t_frequent_contacts (   
	guid character varying(64) NOT NULL,
	staff_code character varying(64),
	contact_number character varying(64),
	contact_name character varying(128),	
	contact_dept character varying(128),	
	contact_number2 character varying(64),
	contact_number3 character varying(64),
	contact_number4 character varying(64),
	create_time timestamp(6) without time zone,	
	update_time timestamp(6) without time zone,
	contact_guid character varying(64) NOT NULL 
	);
ALTER TABLE ONLY icc_t_frequent_contacts ADD CONSTRAINT icc_t_frequent_contacts_pkey PRIMARY KEY (guid,contact_guid);

DROP TABLE IF EXISTS "public"."icc_t_major_alarm_config";
CREATE TABLE icc_t_major_alarm_config (   
	guid character varying(64) NOT NULL,
	type_name character varying(64),
	type_value character varying(64),
	remark character varying(1024)
	);
ALTER TABLE ONLY icc_t_major_alarm_config ADD CONSTRAINT icc_t_major_alarm_config_pkey PRIMARY KEY (guid);
ALTER TABLE ONLY icc_t_major_alarm_config ADD CONSTRAINT unique_type_name_type_value UNIQUE (type_name, type_value);
DROP TABLE IF EXISTS "public"."icc_t_acd_dept";
CREATE TABLE icc_t_acd_dept (   
	acd character varying(64) NOT NULL,
	dept_code character varying(64),
	dept_name character varying(64),
	dept_code_path character varying(64),
	dept_name_path character varying(64),
	remark character varying(512),
	update_time timestamp(6) without time zone
	);
ALTER TABLE ONLY icc_t_acd_dept ADD CONSTRAINT icc_t_acd_dept_pkey PRIMARY KEY (acd);

DROP TABLE IF EXISTS "public"."icc_t_process_seat_telephones";
CREATE TABLE icc_t_process_seat_telephones (   
	guid character varying(64) NOT NULL,
	seats_guid character varying(64),
	org_code character varying(64),
	org_name character varying(128),	
	telephone character varying(64),
	ip character varying(64),
	update_time timestamp(6) without time zone
	);
ALTER TABLE ONLY icc_t_process_seat_telephones ADD CONSTRAINT icc_t_process_seat_telephones_pkey PRIMARY KEY (guid);
CREATE INDEX icc_t_process_seat_telephones_telephone ON icc_t_process_seat_telephones USING btree (telephone);

DROP TABLE IF EXISTS "public"."icc_t_shift_relation";
CREATE TABLE icc_t_shift_relation (
  shift_id varchar(64) NOT NULL,
  alarm_id varchar(64) NOT NULL
);
ALTER TABLE ONLY icc_t_shift_relation ADD CONSTRAINT icc_t_shift_relation_pkey PRIMARY KEY (shift_id, alarm_id);


SET default_tablespace = '';
SET default_with_oids = false;

CREATE VIEW icc_view_dept_recursive AS SELECT 
    guid,
    parent_guid,
    code,
    name,
    path,
    depth
   FROM icc_t_dept;
  
CREATE OR REPLACE VIEW icc_view_user_bind_func AS SELECT
	u.code,
	b.from_guid,
	c.to_guid,
	c.type,
	f.code as func_code,
	f.name 
FROM
	icc_t_user u
	LEFT JOIN icc_t_bind b ON u.guid = b.from_guid AND b.TYPE = 'user_bind_role'
	LEFT JOIN icc_t_bind C ON c.from_guid = b.to_guid AND C.TYPE = 'role_bind_func'
	LEFT JOIN icc_t_func f ON f.guid = c.to_guid ;

CREATE OR REPLACE VIEW icc_view_user_bind_data AS SELECT
	u.code,
    u.guid as from_guid,
    d.guid as to_guid,	
	d.parent_guid,
	d.code AS dept_code,
	d.district_code,
    d.name,
    d.type AS dept_type,
	d.phone,
	d.level,
	d.shortcut,
	d.sort
FROM
	icc_t_user u	
	left JOIN icc_t_staff S ON s.guid = u.staff_guid
	LEFT JOIN icc_t_dept d ON s.dept_guid = d.guid;
	
CREATE OR REPLACE VIEW icc_view_user_bind_staff AS 
SELECT
	u.code,
	u.guid as from_guid,
	u.staff_guid as to_guid,	
	s.code as staff_code,
	s.name,
	d.code as dept_code,
	d.name as dept_name
FROM
	icc_t_user u	
	LEFT JOIN icc_t_staff s ON s.guid = u.staff_guid
	LEFT JOIN icc_t_dept d ON d.guid = s.dept_guid;

	
 CREATE OR REPLACE VIEW icc_view_dict_zh_cn AS	SELECT d.guid,    
    d.dict_key as code,
    d.parent_key AS parent_code,
    d.is_delete,
    d.sort,
    d.value,    
    d.create_time AS value_create_time
   FROM icc_t_dict_value d where lower(d.lang_code) = 'zh-cn' ORDER BY d.dict_key;
   
 CREATE OR REPLACE VIEW icc_view_dict_en_us AS	SELECT d.guid,    
    d.dict_key as code,
    d.parent_key AS parent_code,
    d.is_delete,
    d.sort,
    d.value,    
    d.create_time AS value_create_time
   FROM icc_t_dict_value d where lower(d.lang_code) = 'en' ORDER BY d.dict_key;   


CREATE OR REPLACE VIEW icc_view_hwb AS	
SELECT 
	COALESCE(c.receipt_dept_district_code, ''::character varying) AS xzqh,
    ((COALESCE(c.receipt_dept_district_code, ''::character varying))::text || (a.callref_id)::text) AS hrbh,
    ((c.receipt_dept_district_code)::text || (c.id)::text) AS jjdbh,
    a.ring_time AS hrsj,
    a.talk_time AS zjsj,
    a.hangup_time AS gjsj,
    a.caller_id AS zjhm,
    a.acd AS bjhm,
    ''::text AS lybh,
    ''::text AS lycflj,
    ''::text AS zjyh,
    ''::text AS zjdz,
    ''::text AS jjyxm,
    ''::text AS zlsc,
    ''::text AS thsc,
    '1'::text AS lywjgs
FROM 
	((icc_t_callevent a
	LEFT JOIN icc_t_alarm c ON (((c.source_id)::text = (a.callref_id)::text)))
	LEFT JOIN icc_t_post_data_message m ON (((m.datakey)::text = ((c.receipt_dept_district_code)::text || (a.callref_id)::text))))
	WHERE ((c.id IS NOT NULL) 
		AND ((a.call_direction)::text = 'in'::text) 
		AND (a.talk_time > (now() - '7 days'::interval)) 
		AND (a.talk_time < (now() - '00:15:00'::interval)) 
		AND ((((m.type)::text = 'CallInfoData'::text) 
			AND ((m.status)::text <> 'OK'::text)) 
			OR (m.id IS NULL) 
			OR (a.talk_time > (now() - '01:00:00'::interval))));

CREATE OR REPLACE VIEW icc_view_jjb AS			
SELECT 
	COALESCE(m_gxdw.district_code, ''::character varying) AS xzqh,
    ((COALESCE(m_gxdw.district_code, ''::character varying))::text || (c.id)::text) AS jjdbh,
    c.merge_id AS gljjdbh,
    COALESCE(m_caseresource.peer_code, '9'::character varying) AS bjfsdm,
    COALESCE(c.receipt_dept_code, m_jjdw.code) AS jjdwdm,
    COALESCE(c.receipt_code, m_user.code) AS jjrjh,
    COALESCE(c.receipt_name, m_user.name) AS jjrxm,
    c.leader_code AS zbldjh,
    c.leader_name AS zbldxm,
    c."time" AS bjsj,
    c.caller_no AS bjdhhm,
    COALESCE(c.caller_name, '匿名'::character varying) AS yhxm,
    COALESCE(c.caller_addr, '无'::character varying) AS yhdz,
    COALESCE(m_caseresource.peer_code, '9'::character varying) AS sjjjlxdm,
    '0'::text AS swbj,
    COALESCE(c.contact_name, '匿名'::character varying) AS bjrxm,
    COALESCE(m_sex.peer_code, '2'::character varying) AS bjrxb,
    COALESCE(c.contact_age, '0'::character varying) AS bjrnl,
    COALESCE(m_casehandletype.peer_code, '9'::character varying) AS lhlbdm,
    c.contact_no AS lxdh,
    c.addr AS afdd,
    COALESCE(c.content, '无'::text) AS bjnr,
    c.symbol_code AS xcbsbh,
    c.symbol_addr AS xcbswz,
    COALESCE(c.admin_dept_code, '999999999999'::character varying) AS gxdwdm,
    COALESCE(m_casetype.peer_code, '990000'::character varying) AS sjlbdm,
    COALESCE(m_casesubtype.peer_code, '990000'::character varying) AS sjlxdm,
    COALESCE(m_casethreetype.peer_code, '990000'::character varying) AS sjxldm,
    COALESCE(m_casefourype.peer_code, '990000'::character varying) AS xsjqxldm,
    ''::text AS bjcph,
    '0'::text AS bkrs,
    '0'::text AS ssrs,
    '0'::text AS swrs,
    ''::text AS jzlbdm,
    '0'::text AS yfwxp,
    '0'::text AS yfzsty,
    COALESCE(m_caselevel.peer_code, '4'::character varying) AS jqjb,
    '3'::text AS jjdzt,
    COALESCE(c.admin_dept_code, '999999999999'::character varying) AS zbdwdm,
    ''::text AS zjly,
    m_sgcllxdm.peer_code AS sgcllxdm,
    c.vehicle_no AS sgclcph,
    COALESCE(c.update_time, c.create_time) AS gxsj,
    ''::text AS bjrzjlx,
    ''::text AS bjrzjhm,
    COALESCE(m_jz.peer_code, '1'::character varying) AS jz,
    decode(VARIADIC ARRAY[substr((m_jjdw.code)::text, 1, 4), '530'::text, (substr((m_jjdw.code)::text, 1, 7) || '000000'::text), '999999999999'::text]) AS ssfj,
    ''::text AS xzb,
    ''::text AS yzb,
    c."time" AS jjkssj,
    c."time" AS jjjssj,
    ''::text AS srjzsj,
    ''::text AS jzjssj,
    ''::text AS jzcljg,
    ''::text AS jzclzt,
    '0'::text AS sfsbzdasj,
    ''::text AS sbrxm,
    ''::text AS sbrjh,
    ''::text AS sbsj,
    ''::text AS ccss,
    ''::text AS xjss,
    ''::text AS tsry,
    ''::text AS tscl,
    ''::text AS shrzh,
    ''::text AS shrhm,
    ''::text AS xtrzh,
    ''::text AS xyrhm,
    ''::text AS zzjr,
    ''::text AS zzkssj,
    ''::text AS zzjssj,
    ''::text AS bjlydm
FROM 
	(((((((((((((((icc_t_alarm c
	LEFT JOIN icc_t_alarm_process p ON (((c.id)::text = (p.alarm_id)::text)))
	LEFT JOIN icc_t_dept m_gxdw ON (((c.admin_dept_code)::text = (m_gxdw.code)::text)))
	LEFT JOIN icc_t_staff m_user ON (((c.receipt_code)::text = (m_user.code)::text)))
	LEFT JOIN icc_t_dept m_jjdw ON (((m_user.dept_guid)::text = (m_jjdw.guid)::text)))
	LEFT JOIN icc_t_dict_code_map m_sex ON (((c.contact_gender)::text = (m_sex.dict_code)::text)))
	LEFT JOIN icc_t_dict_code_map m_caseresource ON (((c.source_type)::text = (m_caseresource.dict_code)::text)))
	LEFT JOIN icc_t_dict_code_map m_casehandletype ON (((c.handle_type)::text = (m_casehandletype.dict_code)::text)))
	LEFT JOIN icc_t_dict_code_map m_casetype ON ((((c.first_type)::text = (m_casetype.dict_code)::text) AND ((m_casetype.level)::text = '1'::text))))
	LEFT JOIN icc_t_dict_code_map m_casesubtype ON ((((c.second_type)::text = (m_casesubtype.dict_code)::text) AND ((m_casesubtype.level)::text = '2'::text))))
	LEFT JOIN icc_t_dict_code_map m_casethreetype ON ((((c.third_type)::text = (m_casethreetype.dict_code)::text) AND ((m_casethreetype.level)::text = '3'::text))))
	LEFT JOIN icc_t_dict_code_map m_casefourype ON ((((c.fourth_type)::text = (m_casefourype.dict_code)::text) AND ((m_casefourype.level)::text = '4'::text))))
	LEFT JOIN icc_t_dict_code_map m_caselevel ON (((c.level)::text = (m_caselevel.dict_code)::text)))
	LEFT JOIN icc_t_dict_code_map m_sgcllxdm ON (((c.vehicle_type)::text = (m_sgcllxdm.dict_code)::text)))
	LEFT JOIN icc_t_dict_code_map m_jz ON (((c.called_no_type)::text = (m_sgcllxdm.dict_code)::text)))
	LEFT JOIN icc_t_post_data_message ON (((icc_t_post_data_message.datakey)::text = ((COALESCE(m_gxdw.district_code, ''::character varying))::text || (c.id)::text))))
WHERE ((c.id IS NOT NULL) 
	AND (c."time" > (now() - '60 days'::interval)) 
	AND (c."time" < (now() - '00:15:00'::interval)) 
	AND (((m_casehandletype.peer_code)::text <> '1'::text) 
	OR (((m_casehandletype.peer_code)::text = '1'::text) 
	AND (m_gxdw.code IS NOT NULL) AND (c.first_type IS NOT NULL))) 
	AND ((((icc_t_post_data_message.type)::text = 'AcceptData'::text) 
		AND ((icc_t_post_data_message.status)::text <> 'OK'::text)) 
		OR (icc_t_post_data_message.id IS NULL) 
		OR (c.update_time > icc_t_post_data_message.posttime) 
		OR (c."time" > (now() - '01:00:00'::interval))));

CREATE OR REPLACE VIEW icc_view_cjb AS		
SELECT 
	COALESCE(dep_pj.district_code, ''::character varying) AS xzqh,
    ((COALESCE(dep_pj.district_code, ''::character varying))::text || (p.id)::text) AS cjdbh,
    ((COALESCE(dep_pj.district_code, ''::character varying))::text || (p.alarm_id)::text) AS jjdbh,
    p.time_submit AS pjsj,
    decode(VARIADIC ARRAY[substr((dep_pj.code)::text, 1, 4), '530'::text, (dep_pj.code)::text, NULL::text, ''::text, '999999999999'::text]) AS pjdwdm,
    p.dispatch_code AS pjrjh,
    p.dispatch_name AS pjrxm,
    p.dispatch_leader_code AS pjdwzbldjh,
    p.dispatch_leader_name AS pjdwzbldxm,
    decode(VARIADIC ARRAY[substr((p.process_dept_code)::text, 1, 4), '530'::text, (p.process_dept_code)::text, NULL::text, ''::text, '999999999999'::text]) AS sjdwdm,
    p.process_code AS sjrjh,
    p.process_name AS sjrxm,
    decode(VARIADIC ARRAY[substr((p.process_dept_code)::text, 1, 4), '530'::text, (p.process_dept_code)::text, NULL::text, ''::text, '999999999999'::text]) AS cjdwdm,
    p.process_code AS cjrjh,
    p.process_name AS cjrxm,
    p.dispatch_suggestion AS pjyj,
    p.dispatch_leader_instruction AS ldzs,
    '3'::text AS pjdzt,
    p.time_edit AS pjdtxsj,
    p.time_submit AS pjdscsj,
    p.time_arrived AS sjdwxtddsj,
    p.time_signed AS sjdwqssj,
    COALESCE(p.update_time, p.time_submit) AS gxsj,
    ''::text AS imei,
    0 AS zdfkbj,
    0 AS fkcsbj,
    0 AS wxfkbj
FROM 
	(((((icc_t_alarm_process p
     LEFT JOIN icc_t_alarm c ON (((p.alarm_id)::text = (c.id)::text)))
     LEFT JOIN icc_t_staff user_pj ON (((user_pj.code)::text = (p.dispatch_code)::text)))
     LEFT JOIN icc_t_dept dep_pj ON (((dep_pj.guid)::text = (user_pj.dept_guid)::text)))
     LEFT JOIN icc_t_dict_code_map m_pjdzt ON (((p.state)::text = (m_pjdzt.dict_code)::text)))
     LEFT JOIN icc_t_post_data_message ON ((((icc_t_post_data_message.datakey)::text = ((COALESCE(p.dispatch_dept_district_code, ''::character varying))::text || (p.id)::text)) AND ((icc_t_post_data_message.type)::text = 'DispatchData'::text))))
 WHERE ((p.id IS NOT NULL) 
	AND (p.time_submit > (now() - '60 days'::interval)) 
	AND (p.time_submit < (now() - '00:15:00'::interval)) 
	AND ((icc_t_post_data_message.id IS NULL) 
		OR ((icc_t_post_data_message.status)::text <> 'OK'::text) 
		OR (p.update_time > icc_t_post_data_message.posttime) 
		OR (p.time_submit > (now() - '01:00:00'::interval))));

CREATE OR REPLACE VIEW icc_view_fkb AS		
SELECT 
	COALESCE(f.feedback_dept_district_code, ''::character varying) AS xzqh,
    ((COALESCE(f.feedback_dept_district_code, ''::character varying))::text || (f.id)::text) AS fkdbh,
    ((COALESCE(f.feedback_dept_district_code, ''::character varying))::text || (f.alarm_id)::text) AS jjdbh,
    ((COALESCE(f.feedback_dept_district_code, ''::character varying))::text || (f.process_id)::text) AS cjdbh,
    f.time_submit AS fksj,
    decode(VARIADIC ARRAY[substr((f.feedback_dept_code)::text, 1, 4), '530'::text, (f.feedback_dept_code)::text, NULL::text, ''::text, '999999999999'::text]) AS fkdwdm,
    f.feedback_leader_code AS fkdwzbldjh,
    f.feedback_leader_name AS fkdwzbldxm,
    f.feedback_code AS fkrjh,
    COALESCE(fkr_uere.name, f.feedback_code) AS fkrxm,
    decode(VARIADIC ARRAY[substr((f.process_dept_code)::text, 1, 4), '530'::text, (f.process_dept_code)::text, NULL::text, ''::text, '999999999999'::text]) AS cjdwdm,
    f.process_leader_code AS cjdwzbldjh,
    f.process_leader_name AS cjdwzbldxm,
    f.process_code AS cjrjh,
    COALESCE(f.process_name, f.process_code) AS cjrxm,
    f.time_police_dispatch AS sjcjsj,
    f.time_police_arrived AS ddxcsj,
    f.person_id AS dsrzjh1,
    f.person_nationality AS dsrgj1,
    f.person_name AS dsrxm1,
    f.person_slave_id AS dsrzjh2,
    f.person_slave_nationality AS dsrgj2,
    f.person_slave_name AS dsrxm2,
    ''::text AS dsrzjh3,
    ''::text AS dsrgj3,
    ''::text AS dsrxm3,
    f.actual_occur_time AS sjfssj,
    COALESCE(m_casetype.peer_code, '990000'::character varying) AS sjlbdm,
    COALESCE(m_casesubtype.peer_code, '990000'::character varying) AS sjlxdm,
    COALESCE(m_casethreetype.peer_code, '990000'::character varying) AS sjxldm,
    COALESCE(m_casefourype.peer_code, '990000'::character varying) AS xsjqxldm,
    COALESCE(m_sfdzlbdm.peer_code, '99'::character varying) AS sfdzlbdm,
    COALESCE(m_sfdzlxdm.peer_code, ''::character varying) AS sfdzlxdm,
    COALESCE(f.actual_occur_addr, '无'::text) AS sfdzxxms,
    f.alarm_addr_dept_name AS sfddwmc,
    COALESCE(f.alarm_longitude, (0)::double precision) AS sfdxzb,
    COALESCE(f.alarm_latitude, (0)::double precision) AS sfdyzb,
    COALESCE(m_sfbwlxdm.peer_code, '99'::character varying) AS sfbwlxdm,
    COALESCE(m_sfcslxdm.peer_code, ''::character varying) AS sfcslxdm,
    COALESCE(f.people_num_capture, 0) AS zhrs,
    COALESCE(f.people_num_rescue, 0) AS jzrs,
    '0'::text AS sfphxsaj,
    '0'::text AS sfcczaaj,
    COALESCE(f.police_car_num_dispatch, 0) AS cdcls,
    COALESCE(f.police_num_dispatch, 0) AS cdjl,
    trunc(COALESCE(f.economy_loss, (0)::double precision)) AS jjss,
    COALESCE(f.people_num_slight_injury, 0) AS qsrs,
    COALESCE(f.people_num_serious_injury, 0) AS zsrs,
    COALESCE(f.people_num_death, 0) AS swrs,
    round(COALESCE(f.retrieve_economy_loss, (0)::double precision)) AS whjjss,
    COALESCE(m_cljgdm.peer_code, '9'::character varying) AS cljgdm,
    decode(VARIADIC ARRAY[(COALESCE(m_fkdzt.peer_code, ''::character varying))::text, '10'::text, '10'::text, '20'::text, '20'::text, '30'::text, '30'::text, '30'::text]) AS fkdzt,
    ''::text AS jsdwqszt,
    f.time_police_dispatch AS cjsj,
    f.process_code AS jsrjh,
    f.process_name AS jsrxm,
    f.fire_put_out_time AS hcpmsj,
    m_jzlbdm.peer_code AS jzlbdm,
    decode(VARIADIC ARRAY[(''::text || length((COALESCE(m_qhcsxldm.peer_code, ''::character varying))::text)), '3'::text, substr((m_qhcsxldm.peer_code)::text, 1, 3), (m_qhcslxdm.peer_code)::text]) AS qhcslxdm,
    m_qhcsxldm.peer_code AS qhcsxldm,
    m_qhydm.peer_code AS qhydm,
    m_hzyylbdm.peer_code AS hzyylbdm,
    m_hzyylxdm.peer_code AS hzyylxdm,
    m_hzyyxldm.peer_code AS hzyyxldm,
    COALESCE(f.fire_area, (0)::double precision) AS ghmj,
    m_dldjdm.peer_code AS dldjdm,
    m_sgjbdm.peer_code AS sgjbdm,
    m_sgcllxdm.peer_code AS sgcllxdm1,
    f.traffic_vehicle_no AS sgclcph1,
    m_sgcllxdm2.peer_code AS sgcllxdm2,
    f.traffic_slave_vehicle_no AS sgclcph2,
    ''::text AS sgcllxdm3,
    ''::text AS sgclcph3,
    COALESCE(f.update_time, f.create_time) AS gxsj,
    COALESCE(f.result_content, '无'::text) AS cljgnr,
    COALESCE(f.leader_instruction, '无'::text) AS ldzs,
    decode(VARIADIC ARRAY[substr((f.feedback_dept_code)::text, 1, 4), '530'::text, (f.feedback_dept_code)::text, NULL::text, ''::text, '999999999999'::text]) AS fktxdw,
    f.feedback_code AS fktxrjh,
    COALESCE(fkr_uere.name, f.feedback_code) AS fktxrxm,
    ''::text AS zjbz,
    decode(VARIADIC ARRAY[(m_fklx.peer_code)::text, ''::text, '30'::text, '50'::text, '50'::text, '40'::text, '40'::text, '30'::text]) AS fklx,
    ''::text AS sfsw,
    ''::text AS sfsj,
    ''::text AS sjafdz,
    ''::text AS sjafsj,
    ''::text AS qhbwdm
FROM 
	((((((((((((((((((((((((((icc_t_alarm_feedback f
     LEFT JOIN icc_t_alarm c ON (((c.id)::text = (f.alarm_id)::text)))
     LEFT JOIN icc_t_alarm_process p ON (((p.id)::text = (f.process_id)::text)))
     LEFT JOIN icc_t_dict_code_map m_casetype ON ((((f.alarm_first_type)::text = (m_casetype.dict_code)::text) AND ((m_casetype.level)::text = '1'::text))))
     LEFT JOIN icc_t_dict_code_map m_casesubtype ON ((((f.alarm_second_type)::text = (m_casesubtype.dict_code)::text) AND ((m_casesubtype.level)::text = '2'::text))))
     LEFT JOIN icc_t_dict_code_map m_casethreetype ON ((((f.alarm_third_type)::text = (m_casethreetype.dict_code)::text) AND ((m_casethreetype.level)::text = '3'::text))))
     LEFT JOIN icc_t_dict_code_map m_casefourype ON ((((f.alarm_fourth_type)::text = (m_casefourype.dict_code)::text) AND ((m_casefourype.level)::text = '4'::text))))
     LEFT JOIN icc_t_dict_code_map m_sfdzlbdm ON (((m_sfdzlbdm.dict_code)::text = (f.alarm_addr_first_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_sfdzlxdm ON (((m_sfdzlxdm.dict_code)::text = (f.alarm_addr_second_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_sfbwlxdm ON (((m_sfbwlxdm.dict_code)::text = (f.alarm_region_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_sfcslxdm ON (((m_sfcslxdm.dict_code)::text = (f.alarm_location_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_cljgdm ON (((m_cljgdm.dict_code)::text = f.result_type)))
     LEFT JOIN icc_t_dict_code_map m_fkdzt ON (((m_fkdzt.dict_code)::text = (f.state)::text)))
     LEFT JOIN icc_t_dict_code_map m_jzlbdm ON (((m_jzlbdm.dict_code)::text = (f.fire_building_first_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_qhcslxdm ON (((m_qhcslxdm.dict_code)::text = (f.fire_building_second_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_qhcsxldm ON (((m_qhcsxldm.dict_code)::text = (f.fire_building_third_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_qhydm ON (((m_qhydm.dict_code)::text = (f.fire_source_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_hzyylbdm ON (((m_hzyylbdm.dict_code)::text = (f.fire_cause_first_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_hzyylxdm ON (((m_hzyylxdm.dict_code)::text = (f.fire_cause_second_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_hzyyxldm ON (((m_hzyyxldm.dict_code)::text = (f.fire_cause_third_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_dldjdm ON (((m_dldjdm.dict_code)::text = (f.traffic_road_level)::text)))
     LEFT JOIN icc_t_dict_code_map m_sgjbdm ON (((m_sgjbdm.dict_code)::text = (f.traffic_accident_level)::text)))
     LEFT JOIN icc_t_dict_code_map m_sgcllxdm ON (((m_sgcllxdm.dict_code)::text = (f.traffic_vehicle_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_sgcllxdm2 ON (((m_sgcllxdm2.dict_code)::text = (f.traffic_slave_vehicle_type)::text)))
     LEFT JOIN icc_t_dict_code_map m_fklx ON (((m_fklx.dict_code)::text = f.result_type)))
     LEFT JOIN icc_t_post_data_message ON ((((icc_t_post_data_message.datakey)::text = ((COALESCE(f.feedback_dept_district_code, ''::character varying))::text || (f.id)::text)) AND ((icc_t_post_data_message.type)::text = 'FeedBackData'::text))))
     LEFT JOIN icc_t_staff fkr_uere ON (((fkr_uere.code)::text = (f.feedback_code)::text)))
WHERE ((p.id IS NOT NULL) 
	AND (f.time_submit IS NOT NULL) 
	AND (f.time_submit > (now() - '60 days'::interval)) 
	AND (f.time_submit < (now() - '00:15:00'::interval)) 
	AND (((icc_t_post_data_message.status)::text <> 'OK'::text) 
		OR (icc_t_post_data_message.id IS NULL) 
		OR (f.update_time > icc_t_post_data_message.posttime) 
		OR (f.time_submit > (now() - '01:00:00'::interval))));


INSERT INTO icc_t_param VALUES ('SystemID', 'ICC', '0', '1', 'sys001','', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('autoBusy', '0', '1', '0','坐席接警时是否自动置忙, 0为不自动置忙，1为自动置忙。', 'sys002', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('busyNeedApply', '1', '0', '0','离席置忙是否需要申请, 0为不需要，1为需要', 'sys003', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('NTPTime_serverIp', '127.0.0.1', '0', '0', 'sys004', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('mqUserName', 'admin', '1', '0','mq登录用户名', 'sys005', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('mqPassword', 'Command_123', '1', '0','mq用户密码', 'sys006', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('mrpsUserName', 'icc', '1', '0','MRPS用户名', 'sys007', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('MrpsPwd', 'MRPS_123456', '1', '0','MRPS密码', 'sys008', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('seatNoCanrepeat', 'false', '1', '0','同一个坐席号是否可以多个前端登录', 'sys009', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('setHeartTimeSpan', '5000', '1', '0','设置心跳初始间隔时间', 'sys011', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('deptBusinessCode', 'CJFZ', '1', '0','SMP资源树businessCode', 'sys012', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('supportEditing', '[1, 2, 3]', '1', '0','是否支持编辑,[],默认0,-1：不支持编辑，0：所有人，1：当前座席，2：当前部门，3：当前警员，支持多种组合配置，后续可扩展其他情形(需代码逻辑支持)', 'sys013', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('autoSendSmsTime', '30', '1', '0','单位（秒） 来电创警自动发送短信定位的时间延迟', 'sys014', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('sendSmsContentLength', '200', '1', '0','创警发送短信内容的最大长度', 'sys015', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('messageAlertTimeOut', '10', '1', '0','接收到提示消息时，提示框自动消失时长（妙）', 'sys016', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('isUpAttention', '1', '0', '1','今日警情查询列表关注警单是否置顶，1是，0否', 'sys017', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('iccOctgTimeout', '500', '1', '0','非话务队列排队超时设置，单位秒(s)', 'sys018', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('callQuqueTimeOut', '50', '1', '0',' 电话队列排队超时时长配置，单位秒(s)', 'sys019', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('enableBigDataAPI', 'false', '1', '0','是否启用大数据接口对接', 'sys020', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('sendChatMsg', 'false', '1', '0','提交警单时，如果报警内容变化，是否发送群聊消息', 'sys021', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('callEventPageSize', '20', '1', '0','坐席早释电话/最近呼叫列表请求分页参数,最小有效设置为20', 'sys022', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('callCreateAlarmMode', '0', '1', '0','来电创建警单模式,0.内部电话呼叫创建警单，1.带ACD分组信息呼叫创建警单,默认值为0', 'sys023', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('enableNLPAPI', 'false', '1', '0','是否启用NLP接口对接', 'sys024', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('BusyOverTime', '坐席($)置忙超时', '0', '1','', 'sys030', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('CallRingOverTime', '坐席($)振铃超时', '0', '1', '', 'sys031', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('CallTalkOverTime', '坐席($)通话超时', '0', '1','',  'sys032', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('TimeOver', '超时', '0', '1','',  'sys033', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('AlarmChange', '您关注的警单($)内容变更', '0', '1','',  'sys034', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('AlarmTypeChange', '本单位警单($)类型变更', '0', '1','',  'sys035', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('PrivacyProtection', '警员($)申请取消警单($)的隐私保护!', '0', '1','',  'sys036', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('ApprovalMsgAlarmUp', '警员($)申请警情($)升级至($)级,原因:($)', '0', '1','',  'sys037', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('BlackApplication', '警员($)申请拉黑电话($)', '0', '1','',  'sys038', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('AddMajorAlarmConfig', '坐席($)新增重大警情($)', '0', '1','',  'sys039', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('AddMajorAlarmConfigMax', '坐席($)新增重大警情,当警单内容中包含($)', '0', '1','',  'sys040', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('AlarmOverTime', '坐席($)接警超时,警单号($)', '0', '1','',  'sys041', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('ManualAccept', '您关注的警单($)已被警员($)签收', '0', '1','',  'sys042', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('Handle_CaseDone', '您关注的警单($)已处置完成', '0', '1','',  'sys043', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('Handle_SiteDone', '您关注的警单($)处警员($)现场处置完毕', '0', '1','',  'sys044', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('Feedback_Text', '您关注的警单($)提交反馈', '0', '1','',  'sys045', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('DispatchOrg_AcceptTimeOut', '您关注的警单($)派单签收超时', '0', '1','',  'sys046', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('DispatchOrg_SendTimeOut', '您关注的警单($)派单超时', '0', '1','',  'sys047', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('Handle_ArriveTimeOut', '您关注的警单($)到达现场超时', '0', '1','',  'sys048', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('VcsMsg', '您关注的警单($)处警内容变更', '0', '1','',  'sys049', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('Dispatch_Send', '您关注的警单($)由($)派出警员($)进行处理', '0', '1','',  'sys050', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('ManualAcceptOrg', '您关注的警单($)已被($)签收', '0', '1','',  'sys051', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('Handle_SiteDoneTimeOut', '您关注的警单($)处警员($)现场处置完毕超时', '0', '1','',  'sys052', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('Handle_CaseDoneTimeOut', '您关注的警单($)警情处置完毕超时', '0', '1','',  'sys053', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('Handle_Arrive', '您关注的警单($)处警员($)已到达现场', '0', '1', '', 'sys054', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('AlarmOverTimePerson', '警员($)接警超时,警单号($)', '0', '1','',  'sys055', now(), NULL, NULL);
INSERT INTO icc_t_param VALUES ('Dispatch_AcceptTimeOut', '您关注的警单($)调派签收超时', '0', '1','',  'sys056', now(), NULL, NULL);
--2023-02-27(新增12345反馈发送至消息中心信息)
INSERT INTO icc_t_param(name, value) VALUES ('12345FlowPath', '非警务反馈：警单($)到达新的流程($)');
INSERT INTO icc_t_param(name, value) VALUES ('12345Extension', '非警务反馈：警单($)申请延期处理，申请时限($)');
INSERT INTO icc_t_param(name, value) VALUES ('12345State', '非警务反馈：警单($)当前办理状态($)');
INSERT INTO icc_t_param(name, value) VALUES ('12345ReturnVisit', '非警务反馈：警单($)收到回访信息');
INSERT INTO icc_t_param(name, value) VALUES ('12345Result', '非警务反馈：警单($)收到反馈信息，当前反馈状态：($)');
INSERT INTO icc_t_param(name, value) VALUES ('12345Repulse', '非警务反馈：警单($)收到退回信息');
--2023-03-06(新增12345反馈发送至消息中心信息)
INSERT INTO icc_t_param(name, value) VALUES ('12345SendError', '非警务反馈：警单($)推送至12345失败');
--2023-03-10(新增12345确认信息发送至消息中心信息)
INSERT INTO icc_t_param(name, value) VALUES ('12345GetConfirm', '非警务反馈：警单($)已被12345确认接收');
INSERT INTO icc_t_param(name, value) VALUES ('12345NotConfirm', '非警务反馈：警单($)未被12345确认接收');
--2023-03-21(新增坐席离席信息发送至消息中心信息)
INSERT INTO icc_t_param(name, value) VALUES ('DepartureApplication', '警员($)申请($)坐席离席');
INSERT INTO icc_t_param(name, value) VALUES ('DepartureApplicationBack', '($)回复警员($)申请($)坐席离席:($)');
INSERT INTO icc_t_param(name, value) VALUES ('DepartureApplicationResultOk', '同意');
INSERT INTO icc_t_param(name, value) VALUES ('DepartureApplicationResultNo', '拒绝');
--2023-03-21(新增12345信息发送至消息中心信息)
INSERT INTO icc_t_param(name, value) VALUES ('12345ReturnVisitResult1', '满意');
INSERT INTO icc_t_param(name, value) VALUES ('12345ReturnVisitResult2', '基本满意');
INSERT INTO icc_t_param(name, value) VALUES ('12345ReturnVisitResult3', '不满意');
INSERT INTO icc_t_param(name, value) VALUES ('12345ReturnVisitResult4', '无结果');
--2023-04-03(新增自动催办信息发送至消息中心信息)
INSERT INTO icc_t_param(name, value) VALUES ('UrgeDept', '($)对($)发起电话催办');
INSERT INTO icc_t_param(name, value) VALUES ('UrgeDeptSms', '($)对($)发起短信催办');
--2023-04-12(新增定时检查坐席是否有用户绑定消息发送至消息中心)
INSERT INTO icc_t_param(name, value) VALUES ('InitiativeAlarm', '坐席($)没有警员绑定');
--2023-04-18(新增主动检测来电是否有警员绑定坐席消息发送至消息中心)
INSERT INTO icc_t_param(name, value) VALUES ('TelephoneComingAlarm', '坐席($)有电话报警接入，但没有警员绑定该坐席');

--2023-04-17(新增12345信息发送至消息中心信息)
INSERT INTO icc_t_param(name, value) VALUES ('110To12345Repulse', '非警务反馈：警单($)退回至12345，退单警员:($)，警单退回原因:($)');
INSERT INTO icc_t_param(name, value) VALUES ('110To12345RepulseFail', '非警务反馈：警单($)退回至12345失败，退单警员:($)，警单退回原因:($)');
INSERT INTO icc_t_param(name, value) VALUES ('12345RevisitMessage', '非警务反馈：$警员$已反馈,处置结果为$');

-----223.10.23黑名单来电信息推送，发送umc消息到消息中心
INSERT INTO icc_t_param(name, value) VALUES ('BlackMessageSend', '黑名单电话($)拨打号码($)，已自动接至语音提醒');

DROP TABLE IF EXISTS "public"."icc_t_jjdb_log";
CREATE TABLE icc_t_jjdb_log(
	guid character varying(64),
	id character varying(64),
	alarm_id character varying(64),
	alarmtime timestamp(6) without time zone,
	update_user character varying(64),
	alarm_content text
)
;

--2023-02-25
--新增联动单位表
DROP TABLE IF EXISTS "public"."icc_t_linked_dispatch";
CREATE TABLE public.icc_t_linked_dispatch(
	id character varying(64) NOT NULL,
	alarm_id character varying(64) NOT NULL,
	state character varying(64),
	linked_org_code character varying(64),
	linked_org_name character varying(2048),
	linked_org_type character varying(64),
	dispatch_code character varying(64),
	dispatch_name character varying(2048),
	create_user character varying(64),
	create_time timestamp(6) without time zone,
    update_user character varying(64),
    update_time timestamp(6) without time zone,
    result text
);

ALTER TABLE ONLY icc_t_linked_dispatch ADD CONSTRAINT icc_t_linked_dispatch_pkey PRIMARY KEY (id);
COMMENT ON COLUMN public.icc_t_linked_dispatch.id IS '警情id';
COMMENT ON COLUMN public.icc_t_linked_dispatch.alarm_id IS '接警单id';
COMMENT ON COLUMN public.icc_t_linked_dispatch.state IS '处理状态（未推送，已推送，已退单，已作废）';
COMMENT ON COLUMN public.icc_t_linked_dispatch.linked_org_code IS '联动单位代码';
COMMENT ON COLUMN public.icc_t_linked_dispatch.linked_org_name IS '联动单位名称';
COMMENT ON COLUMN public.icc_t_linked_dispatch.linked_org_type IS '联动单位类型';
COMMENT ON COLUMN public.icc_t_linked_dispatch.dispatch_code IS '发起调派警员编号';
COMMENT ON COLUMN public.icc_t_linked_dispatch.dispatch_name IS '发起调派警员名称';
COMMENT ON COLUMN public.icc_t_linked_dispatch.create_user IS '创建者';
COMMENT ON COLUMN public.icc_t_linked_dispatch.create_time IS '创建时间';
COMMENT ON COLUMN public.icc_t_linked_dispatch.update_user IS '更新人';
COMMENT ON COLUMN public.icc_t_linked_dispatch.update_time IS '更新时间';
COMMENT ON COLUMN public.icc_t_linked_dispatch."result" IS '处置结果内容';

DROP TABLE IF EXISTS icc_t_attach;
CREATE TABLE icc_t_attach(
	id character varying(64) NOT NULL,
	alarm_id character varying(64) NOT NULL,
	attach_id character varying(64) NOT NULL,
	attach_name character varying(128),
	attach_type character varying(64),
	attach_path character varying(256),
    attach_source character varying(10),
	create_user character varying(64),
	create_time timestamp(6) without time zone,
	PRIMARY KEY (id)
);

COMMENT ON TABLE icc_t_attach IS '警情附件表';
COMMENT ON COLUMN icc_t_attach.id IS '主键';
COMMENT ON COLUMN icc_t_attach.alarm_id IS '警情id';
COMMENT ON COLUMN icc_t_attach.attach_id IS '警情附件id';
COMMENT ON COLUMN icc_t_attach.attach_name IS '警情附件名字';
COMMENT ON COLUMN icc_t_attach.attach_type IS '警情附件类型';
COMMENT ON COLUMN icc_t_attach.attach_path IS '警情附件路径';
COMMENT ON COLUMN icc_t_attach.attach_source IS '警情附件来源';
COMMENT ON COLUMN icc_t_attach.create_user IS '创建者';
COMMENT ON COLUMN icc_t_attach.create_time IS '创建时间';

-- ----------------------------
-- Table structure for icc_t_wechat_alarm_file
-- ----------------------------
DROP TABLE IF EXISTS "public"."icc_t_wechat_alarm_file";
CREATE TABLE "public"."icc_t_wechat_alarm_file" (
  "jqfjid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
  "jjdbh" varchar(50) COLLATE "pg_catalog"."default",
  "jqfjmc" text COLLATE "pg_catalog"."default",
  "jqfjlx" varchar(5) COLLATE "pg_catalog"."default",
  "jqfjzt" varchar(5) COLLATE "pg_catalog"."default",
  "jqfjly" varchar(5) COLLATE "pg_catalog"."default",
  "fragmentation" varchar(5) COLLATE "pg_catalog"."default",
  "fileid" varchar(32) COLLATE "pg_catalog"."default",
  "filename" varchar(100) COLLATE "pg_catalog"."default",
  "xt_rksj" timestamp(6),
  "xt_gxsj" timestamp(6)
)
;
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."jqfjid" IS '主键';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."jjdbh" IS '接警单编号';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."jqfjmc" IS '警情附件名称';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."jqfjlx" IS '警情附件类型, 附件，音频，视频';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."jqfjzt" IS '警情附件状态，0：已上传，1：新上传';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."jqfjly" IS '警情附件来源，0：报警人上传，1：报警员上传';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."fragmentation" IS '分片，0：不是文件；1：未分片；2：分片';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."fileid" IS '文件ID';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."filename" IS '文件名称';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."xt_rksj" IS '入库时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_file"."xt_gxsj" IS '更新时间';
COMMENT ON TABLE "public"."icc_t_wechat_alarm_file" IS '微信报警附件表';

-- ----------------------------
-- Table structure for icc_t_wechat_alarm_information
-- ----------------------------
DROP TABLE IF EXISTS "public"."icc_t_wechat_alarm_information";
CREATE TABLE "public"."icc_t_wechat_alarm_information" (
  "jjdbh" varchar(32) COLLATE "pg_catalog"."default" NOT NULL,
  "jjdwdm" varchar(20) COLLATE "pg_catalog"."default",
  "jjlx" varchar(5) COLLATE "pg_catalog"."default",
  "jqlyfs" varchar(5) COLLATE "pg_catalog"."default",
  "jjybh" varchar(30) COLLATE "pg_catalog"."default",
  "jjyxm" varchar(20) COLLATE "pg_catalog"."default",
  "bjsj" varchar(20) COLLATE "pg_catalog"."default",
  "jjsj" varchar(20) COLLATE "pg_catalog"."default",
  "bjrmc" varchar(20) COLLATE "pg_catalog"."default",
  "lxdh" varchar(20) COLLATE "pg_catalog"."default",
  "bjdh" varchar(20) COLLATE "pg_catalog"."default",
  "jqdz" varchar(200) COLLATE "pg_catalog"."default",
  "bjnr" varchar(4000) COLLATE "pg_catalog"."default",
  "jqlbdm" varchar(8) COLLATE "pg_catalog"."default",
  "jqlxdm" varchar(8) COLLATE "pg_catalog"."default",
  "jqxldm" varchar(8) COLLATE "pg_catalog"."default",
  "jqzldm" varchar(8) COLLATE "pg_catalog"."default",
  "bjrxzb" float8,
  "bjryzb" float8,
  "sjcph" varchar(100) COLLATE "pg_catalog"."default",
  "cjsj" varchar(20) COLLATE "pg_catalog"."default",
  "gxsj" varchar(20) COLLATE "pg_catalog"."default",
  "sfbm" varchar(5) COLLATE "pg_catalog"."default",
  "bjrxbdm" varchar(5) COLLATE "pg_catalog"."default",
  "bjrzjdm" varchar(5) COLLATE "pg_catalog"."default",
  "bjrzjhm" varchar(96) COLLATE "pg_catalog"."default",
  "lhlx" varchar(5) COLLATE "pg_catalog"."default",
  "jqdjdm" varchar(5) COLLATE "pg_catalog"."default",
  "czlxdm" varchar(5) COLLATE "pg_catalog"."default",
  "sldwdm" varchar(12) COLLATE "pg_catalog"."default",
  "pushstatus" varchar(10) COLLATE "pg_catalog"."default",
  "pushtime" varchar(20) COLLATE "pg_catalog"."default",
  "acktime" varchar(20) COLLATE "pg_catalog"."default",
  "xt_rksj" timestamp(6),
  "xt_gxsj" timestamp(6),
  "seatno" varchar(20) COLLATE "pg_catalog"."default"
)
;
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jjdbh" IS '接警单编号';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jjdwdm" IS '接警单位代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jjlx" IS '接警类型';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jqlyfs" IS '警情来源方式';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jjybh" IS '接警员编号';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jjyxm" IS '接警员姓名';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."bjsj" IS '报警时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jjsj" IS '接警时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."bjrmc" IS '报警人名称';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."lxdh" IS '联系电话';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."bjdh" IS '报警电话';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jqdz" IS '警情地址';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."bjnr" IS '报警内容';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jqlbdm" IS '警情类别代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jqlxdm" IS '警情类型代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jqxldm" IS '警情细类代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jqzldm" IS '警情子类代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."bjrxzb" IS '报警人定位X坐标';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."bjryzb" IS '报警人定位Y坐标';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."sjcph" IS '涉及车牌号';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."cjsj" IS '创建时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."gxsj" IS '更新时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."sfbm" IS '是否保密';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."bjrxbdm" IS '报警人性别代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."bjrzjdm" IS '报警人证件代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."bjrzjhm" IS '来话类型';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."lhlx" IS '来话类型';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."jqdjdm" IS '警情等级代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."czlxdm" IS '处置类型代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."sldwdm" IS '受理单位代码';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."pushstatus" IS '推送状态；0-未推送 1-已推送 2-已处理';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."pushtime" IS '推送时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."acktime" IS '应答时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."xt_rksj" IS '入库时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."xt_gxsj" IS '更新时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_information"."seatno" IS '推送坐席号';
COMMENT ON TABLE "public"."icc_t_wechat_alarm_information" IS '微信报警信息表';

-- ----------------------------
-- Table structure for icc_t_wechat_alarm_message
-- ----------------------------
DROP TABLE IF EXISTS "public"."icc_t_wechat_alarm_message";
CREATE TABLE "public"."icc_t_wechat_alarm_message" (
  "id" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
  "alarmid" varchar(50) COLLATE "pg_catalog"."default",
  "senderid" varchar(50) COLLATE "pg_catalog"."default",
  "sendername" varchar(50) COLLATE "pg_catalog"."default",
  "sendtime" varchar(20) COLLATE "pg_catalog"."default",
  "content" text COLLATE "pg_catalog"."default",
  "receiveid" varchar(32) COLLATE "pg_catalog"."default",
  "receivename" varchar(50) COLLATE "pg_catalog"."default",
  "readtime" varchar(20) COLLATE "pg_catalog"."default",
  "messagetype" varchar(2) COLLATE "pg_catalog"."default",
  "fragmentation" varchar(5) COLLATE "pg_catalog"."default",
  "fileid" varchar(32) COLLATE "pg_catalog"."default",
  "filename" varchar(100) COLLATE "pg_catalog"."default",
  "xt_rksj" timestamp(6),
  "xt_gxsj" timestamp(6)
)
;
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."id" IS '主键';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."alarmid" IS '微信报警单ID';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."senderid" IS '发送人ID';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."sendername" IS '发送人姓名';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."sendtime" IS '开始时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."content" IS '消息内容';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."receiveid" IS '接收人ID';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."receivename" IS '接收人姓名';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."readtime" IS '阅读时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."messagetype" IS '消息类型。1: 普通文本信息;2: 文件信息';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."fragmentation" IS '分片，0：不是文件；1：未分片；2：分片';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."fileid" IS '文件ID';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."filename" IS '文件名称';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."xt_rksj" IS '入库时间';
COMMENT ON COLUMN "public"."icc_t_wechat_alarm_message"."xt_gxsj" IS '更新时间';
COMMENT ON TABLE "public"."icc_t_wechat_alarm_message" IS '微信报警聊天信息表';

-- ----------------------------
-- Indexes structure for table icc_t_wechat_alarm_file
-- ----------------------------
CREATE INDEX "index_jjdbh_wechat_information_file" ON "public"."icc_t_wechat_alarm_file" USING btree (
  "jjdbh" COLLATE "pg_catalog"."default" "pg_catalog"."text_ops" ASC NULLS LAST
);


-- ----------------------------
-- Primary Key structure for table icc_t_wechat_alarm_file
-- ----------------------------
ALTER TABLE "public"."icc_t_wechat_alarm_file" ADD CONSTRAINT "icc_t_wechat_alarm_file_pkey" PRIMARY KEY ("jqfjid");

-- ----------------------------
-- Indexes structure for table icc_t_wechat_alarm_information
-- ----------------------------
CREATE INDEX "index_jjdbh_wechat_information" ON "public"."icc_t_wechat_alarm_information" USING btree (
  "jjdbh" COLLATE "pg_catalog"."default" "pg_catalog"."text_ops" ASC NULLS LAST
);

-- ----------------------------
-- Primary Key structure for table icc_t_wechat_alarm_information
-- ----------------------------
ALTER TABLE "public"."icc_t_wechat_alarm_information" ADD CONSTRAINT "icc_t_wechat_alarm_information_pkey" PRIMARY KEY ("jjdbh");

-- ----------------------------
-- Indexes structure for table icc_t_wechat_alarm_message
-- ----------------------------
CREATE INDEX "index_wechat_alarmId_message" ON "public"."icc_t_wechat_alarm_message" USING btree (
  "alarmid" COLLATE "pg_catalog"."default" "pg_catalog"."text_ops" ASC NULLS LAST
);

-- ----------------------------
-- Primary Key structure for table icc_t_wechat_alarm_message
-- ----------------------------
ALTER TABLE "public"."icc_t_wechat_alarm_message" ADD CONSTRAINT "icc_t_wechat_alarm_message_pkey" PRIMARY KEY ("id");

-----------------------------------------------------------当月分表执行--------------------------------------------------------------
select create_shardtable_icc_t_fkdb() as icc_t_fkdb, create_shardtable_icc_t_jjdb() as icc_t_jjdb, create_shardtable_icc_t_pjdb() as icc_t_pjdb, create_shardtable_icc_t_alarm_log() as icc_t_alarm_log, create_shardtable_icc_t_sub_callevent() as icc_t_sub_callevent, create_shardtable_icc_t_callevent() as icc_t_callevent;
