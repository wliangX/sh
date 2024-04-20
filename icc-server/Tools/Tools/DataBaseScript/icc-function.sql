create extension  if not exists btree_gist;
create extension  if not exists pg_trgm;

-----------------------------------------------------------icc_t_jjdb分表初始化-----------------------------------------------------------
DROP FUNCTION IF exists init_shard_icc_t_jjdb();
CREATE OR REPLACE FUNCTION init_shard_icc_t_jjdb()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	dt_data record;
	back_table_name text := 'icc_t_jjdb_bk';
	final_name text := 'icc_t_jjdb';
	index_name text := 'idx_t_jjdb';
	submeter_key text := 'jjsj';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			XZQHDM character varying(12),
			JJDWDM character varying(12),
			JJDBH character varying(30) NOT NULL,
			GLZJJDBH character varying(30),
			JJLX character varying(2),
			JQLYFS character varying(2),
			LHLX character varying(2),
			JJLYH character varying(255), 
			JJYBH character varying(30),
			JJYXM character varying(50),
			BJSJ timestamp(6) without time zone,
			JJSJ timestamp(6) without time zone, 
			JJWCSJ timestamp(6) without time zone,
			BJDH character varying(50),
			BJDHYHM character varying(50),
			BJDHYHDZ character varying(200),
			BJRMC character varying(200),
			BJRXBDM character varying(2),
			LXDH character varying(50),
			BJRZJDM character varying(32),
			BJRZJHM character varying(96),
			BJDZ character varying(200),
			JQDZ character varying(200),
			BJNR character varying(4000),
			GXDWDM character varying(12),
			JQLBDM character varying(8),
			JQLXDM character varying(8),
			JQXLDM character varying(8),
			JQZLDM character varying(8),
			TZDBH character varying(50),
			ZARS numeric,
			YWCWQ character varying(1),
			YWWXWZ character varying(1),
			YWBZXL character varying(1),
			BKRYQKSM character varying(500),
			SSRYQKSM character varying(500),
			SWRYQKSM character varying(500),
			SFSWYBJ character varying(1),
			BJRXZB numeric,
			BJRYZB numeric,
			FXDWJD numeric,
			FXDWWD numeric,
			BCJJNR character varying(2000),
			JQDJDM character varying(2),
			JQCLZTDM character varying(2),
			YJJYDJDM character varying(2),
			SJCHPZLDM character varying(2),
			SJCPH character varying(100),
			SFSWHCL character varying(1),
			GXSJ timestamp(6) without time zone,
			JQBQ character varying(800),
			SFBM character varying(1),
			GXDWMC character varying(200),
			GXDWDMBS character varying(200),
			DQYWZT character varying(200),
			JJDWMC character varying(100),
			JJDWDMBS character varying(200),
			WXBS smallint DEFAULT 0 NOT NULL, 
			JJXWH character varying(64),
			HBBS smallint DEFAULT 0 NOT NULL, 
			CJRY character varying(64),
			GXRY character varying(64),
			HFBS smallint DEFAULT 0 NOT NULL, 
			FKBS smallint DEFAULT 0 NOT NULL, 
			TZHGXDWDM character varying(200),
			TZHGXDWMC character varying(200),
			TZHGXDWDMBS character varying(200),
			JJFS character varying(8), 
			SCBS smallint DEFAULT 0 NOT NULL, 
			SFTSJZ smallint DEFAULT 1 NOT NULL,
			YBJXXYZ integer default 0,
			TBCJBS integer default 0,
			SFJA smallint DEFAULT 0 NOT NULL,
			GLLX varchar(10), 
			CJZDBS character varying(50),
			GXZDBS character varying(50),
			JJLYLX character varying(30),
			JJLYBH character varying(64),
			BJJJDBM character varying(32),
			BJGLZJJDBM character varying(32),
			XZQHMC character varying(100),
			JCJXTJSDWDM character varying(12),
			JCJXTJSDWMC character varying(100),
			SHLDDWDM character varying(6),
			RKSJ timestamp(6) without time zone,
			SFYQS character varying(1),
			JASJ timestamp(6) without time zone,
			CSJQLBDM character varying(8),
			CSJQLXDM character varying(8),
			CSJQXLDM character varying(8),
			CSJQZLDM character varying(8),
			CSGXDWDM character varying(12),
			CSGXDWMC character varying(200),
			CSGXDWDMBS character varying(200),
			CSJQDZ character varying(200),
			JJDWJC character varying(100),
			GXDWJC character varying(200)
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
		strSQL := 'COMMENT ON COLUMN public.'||final_name||'.XZQHDM
				IS ''行政区划代码(警情所在地行政区划代码，标识到县、市一级。如果县级报警也在市级指挥中心接警，生成警情的行政区划应是报警所在市的行政区划)'';

				COMMENT ON COLUMN public.'||final_name||'.JJDWDM
					IS ''接警单位代码(接警单位一般是当地公安机关110报警服务台，也包括当地自接警单位)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJDBH 
					IS ''接警单编号(接警单编号，产生规则为警情行政区划代码+系统接警单编号，
					以避免不同地区产生同样编号；唯一号，主索引键，并用于关联派警单、反馈单。)'';
					
				COMMENT ON COLUMN public.'||final_name||'.GLZJJDBH 
					IS ''关联主接警单编号(当前接警单所关联的主接警单的编号。)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJLX
					IS ''接警类型(接警类型代码，区分警情属于110、122、自接警和其他接警等)'';
	
				COMMENT ON COLUMN public.'||final_name||'.JQLYFS
					IS ''警情来源方式(警情来源方式，用于说明警情接入的方式，包括电话报警、短信报警、网络报警等。)'';
	
				COMMENT ON COLUMN public.'||final_name||'.LHLX
					IS ''来话类型(来话类型代码，用来描述该来话的基本分类。)'';
	
				COMMENT ON COLUMN public.'||final_name||'.JJLYH
					IS ''接警录音号(接警录音号，接警时由录音系统自动产生，接处警系统自动关联。)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJYBH
					IS ''接警员编号(接警员编号，用来标识接警员身份。)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJYXM
					IS ''接警员姓名'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJSJ
					IS ''报警时间(电话报警时间为排队调度机将报警电话分配到接警台的时间；其他报警方式时间为实际报警时间)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJSJ
					IS ''接警时间(电话报警时为接警员摘电话机应答时间；其余报警方式时为录入电脑开始时间)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJWCSJ
					IS ''接警完成时间(语音报警时为接警员挂断电话机时间，其他报警方式时为保存接警单退出时间)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJDH
					IS ''报警电话(报警电话号码，三字段信息之一)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJDHYHM
					IS ''报警电话用户名(报警电话用户姓名，三字段信息之一，呼入时，由查号系统自动产生)'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJDHYHDZ
					IS ''报警电话用户地址(报警电话用户地址，三字段信息之一，呼入时，由查号系统自动产生)'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJRMC
					IS ''报警人名称(包括互联网报警网名或技防网点名称、设备编号等），考虑外国人报警，留200位，默认为“匿名”)'';
						
				COMMENT ON COLUMN public.'||final_name||'.BJRXBDM
					IS ''报警人性别代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.LXDH
					IS ''联系电话(报警人联系电话，默认为报警电话)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJRZJDM
					IS ''报警人证件代码(报警人证件类型名称)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJRZJHM
					IS ''报警人证件号码(报警人有效证件号码)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJDZ
					IS ''报警地址(报警人所在地址、单位名等。)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQDZ
					IS ''警情地址(警情发生地的地址、单位名等)'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJNR 
					IS ''报警内容(报警内容的详细文字描述)'';
	
				COMMENT ON COLUMN public.'||final_name||'.GXDWDM
					IS ''管辖单位代码(警情管辖受理单位的机构代码)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQLBDM
					IS ''警情类别代码(警情类别代码，用来描述警情性质的一级类型代码)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQLXDM
					IS ''警情类型代码(警情类型代码，用来描述警情性质的二级类型代码)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQXLDM
					IS ''警情细类代码(警情细类代码，用来描述警情性质的三级类型代码)'';
	
				COMMENT ON COLUMN public.'||final_name||'.JQZLDM
					IS ''警情子类代码(警情子类代码，用来描述警情性质的四级类型代码)'';
	
				COMMENT ON COLUMN public.'||final_name||'.TZDBH
					IS ''特征点编号(特征点编号，如路灯杆、视频安装点等辅助定位设施的编号)'';
					
				COMMENT ON COLUMN public.'||final_name||'.ZARS
					IS ''作案人数(违法犯罪嫌疑人数量)'';
					
				COMMENT ON COLUMN public.'||final_name||'.YWCWQ
					IS ''有无持武器(0：无，1：有，默认为0)'';
					
				COMMENT ON COLUMN public.'||final_name||'.YWWXWZ
					IS ''有无危险物质(0：无，1：有，默认为0)'';
					
				COMMENT ON COLUMN public.'||final_name||'.YWBZXL
					IS ''有无爆炸/泄漏(0：无，1：有，默认为0)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BKRYQKSM
					IS ''被困人员情况说明'';
	
				COMMENT ON COLUMN public.'||final_name||'.SSRYQKSM
					IS ''受伤人员情况说明'';
					
				COMMENT ON COLUMN public.'||final_name||'.SWRYQKSM
					IS ''死亡人员情况说明'';
					
				COMMENT ON COLUMN public.'||final_name||'.SFSWYBJ
					IS ''是否是外语报警'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJRXZB
					IS ''报警人定位X坐标'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJRYZB
					IS ''报警人定位Y坐标'';
	
				COMMENT ON COLUMN public.'||final_name||'.FXDWJD
					IS ''反向定位X坐标'';
					
				COMMENT ON COLUMN public.'||final_name||'.FXDWWD
					IS ''反向定位Y坐标'';
					
				COMMENT ON COLUMN public.'||final_name||'.BCJJNR
					IS ''补充接警内容'';	
					
				COMMENT ON COLUMN public.'||final_name||'.JQDJDM
					IS ''警情等级代码'';	
				 
				COMMENT ON COLUMN public.'||final_name||'.JQCLZTDM
					IS ''警情处理状态代码(记录该条警情目前的业务状态)'';	
					
				COMMENT ON COLUMN public.'||final_name||'.YJJYDJDM
					IS ''应急救援等级代码'';
	
				COMMENT ON COLUMN public.'||final_name||'.SJCHPZLDM
					IS ''涉及车号牌种类代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.SJCPH
					IS ''涉及车牌号'';
					
				COMMENT ON COLUMN public.'||final_name||'.SFSWHCL
					IS ''是否是危化车辆(0：否，1：是，默认为0)'';
	
				COMMENT ON COLUMN public.'||final_name||'.GXSJ
					IS ''更新时间(记录新增、变更的时间)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQBQ
					IS ''警情标签(警情标签是对警情内容关键要素的提取和标注。)'';
	
				COMMENT ON COLUMN public.'||final_name||'.SFBM
					IS ''是否保密(0：否，1：是，默认为0)'';  
					
				COMMENT ON COLUMN public.'||final_name||'.JJFS
					IS ''接警服务(区分是vcs自接、icc接警、mpa接警)'';
					
				COMMENT ON COLUMN public.'||final_name||'.GXDWMC
					IS ''管辖单位名称'';	
	
				COMMENT ON COLUMN public.'||final_name||'.GXDWDMBS
					IS ''管辖单位短码'';	
	
				COMMENT ON COLUMN public.'||final_name||'.JJDWMC
					IS ''接警单位名称'';	
	
				COMMENT ON COLUMN public.'||final_name||'.JJDWDMBS
					IS ''接警单位短码'';	
					
				COMMENT ON COLUMN public.'||final_name||'.SCBS
					IS ''删除标记位'';	
	
				COMMENT ON COLUMN public.'||final_name||'.WXBS
					IS ''无效警单标记位'';	
					
				COMMENT ON COLUMN public.'||final_name||'.DQYWZT
					IS ''当前业务状态(vcs业务状态)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJXWH
					IS ''警情接警人席位号'';
					
				COMMENT ON COLUMN public.'||final_name||'.HBBS
					IS ''是否合并(是否合并1：合并，0：未合并)'';	
					
				COMMENT ON COLUMN public.'||final_name||'.CJRY
					IS ''创建人'';	
					
				COMMENT ON COLUMN public.'||final_name||'.GXRY
					IS ''更新人(修改人,取最后一次修改值修改时间,取最后一次修改值)'';
	
				COMMENT ON COLUMN public.'||final_name||'.HFBS
					IS ''是否回访'';
					
				COMMENT ON COLUMN public.'||final_name||'.FKBS
					IS ''是否反馈'';
	
				COMMENT ON COLUMN public.'||final_name||'.gllx 
					IS ''警单关联类型（空或0没关联，1重复报警；2、警情催办；3、民警反馈；4、多次骚扰；）'';
	
				COMMENT ON COLUMN public.'||final_name||'.CJZDBS
					IS ''创建终端标示'';
	
				COMMENT ON COLUMN public.'||final_name||'.GXZDBS
					IS ''更新终端标示'';	
					
				COMMENT ON COLUMN public.'||final_name||'.tzhgxdwdmbs 
					IS ''调整后的管辖单位短码'';
					
				COMMENT ON COLUMN public.'||final_name||'.ybjxxyz
					IS ''是否与背景信息一致 0：不一致，1：一致 ybjxxyz'';
					
				COMMENT ON COLUMN public.'||final_name||'.tbcjbs
					IS ''同步处警标识'';
					
				COMMENT ON COLUMN public.'||final_name||'.tzhgxdwdm
					IS ''调整后的管辖单位代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.tzhgxdwmc 
					IS ''调整后的管辖单位名称'';
					
				COMMENT ON COLUMN public.'||final_name||'.sfja 
					IS ''是否结案'';
				
				COMMENT ON COLUMN public.'||final_name||'.JJLYLX
					IS ''接警来源类型(接警来源类型代码，区分警情属于电话、自接警、12345、微信报警等)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJLYBH
					IS ''接警来源编号(接警来源编号，警情编号属于12345、微信报警的)'';
				
				COMMENT ON COLUMN public.'||final_name||'.BJJJDBM
					IS ''部级接警单编码'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJGLZJJDBM
					IS ''部级关联主接警单编码'';
					
				COMMENT ON COLUMN public.'||final_name||'.XZQHMC
					IS ''行政区划名称'';
					
				COMMENT ON COLUMN public.'||final_name||'.JCJXTJSDWDM
					IS ''接处警系统建设单位代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.JCJXTJSDWMC
					IS ''接处警系统建设单位名称'';
					
				COMMENT ON COLUMN public.'||final_name||'.SHLDDWDM
					IS ''社会联动单位代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.RKSJ
					IS ''入库时间'';
					
				COMMENT ON COLUMN public.'||final_name||'.SFYQS
					IS ''是否已签收'';
					
				COMMENT ON COLUMN public.'||final_name||'.JASJ
					IS ''结案时间'';
					
				COMMENT ON COLUMN public.'||final_name||'.CSJQLBDM
					IS ''初始警情类别代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.CSJQLXDM
					IS ''初始警情类型代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.CSJQXLDM
					IS ''初始警情细类代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.CSJQZLDM
					IS ''初始警情子类代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.CSGXDWDM
					IS ''初始管辖单位代码'';
					
				COMMENT ON COLUMN public.'||final_name||'.CSGXDWMC
					IS ''初始管辖单位名称'';
					
				COMMENT ON COLUMN public.'||final_name||'.CSGXDWDMBS
					IS ''初始管辖单位短码'';
	
				COMMENT ON COLUMN public.'||final_name||'.CSJQDZ
					IS ''初始警情地址'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJDWJC
					IS ''接警单位简称'';
					
				COMMENT ON COLUMN public.'||final_name||'.GXDWJC
					IS ''管辖单位简称'';';
				
	EXECUTE strSQL;

	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (jjdbh);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_addr ON '||final_name||'_'||min_time_begin_short_str||' USING gin (jqdz gin_trgm_ops);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_caller_no ON '||final_name||'_'||min_time_begin_short_str||' USING gin (bjdh gin_trgm_ops);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_content ON '||final_name||'_'||min_time_begin_short_str||' USING gin (bjnr gin_trgm_ops);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_id ON '||final_name||'_'||min_time_begin_short_str||' USING gin (jjdbh gin_trgm_ops);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_source_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jjlyh);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_ten_in_one ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jqclztdm, jqdjdm, lhlx, jqlbdm, jqlxdm, jqxldm, jjlx, gxdwdm, jjdwdm, jjybh);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (bjsj) WITH (pages_per_range=1);
				  create index '||index_name||'_'||min_time_begin_short_str||'_uptime on '||final_name||'_'||min_time_begin_short_str||' (gxsj);
				  create index '||index_name||'_'||min_time_begin_short_str||'_glzjjdbh on '||final_name||'_'||min_time_begin_short_str||' USING btree (glzjjdbh) WHERE glzjjdbh IS NOT NULL;
				  create index '||index_name||'_'||min_time_begin_short_str||'_jjsjtime on '||final_name||'_'||min_time_begin_short_str||' USING brin (jjsj) WITH (pages_per_range=1);
				  create index '||index_name||'_'||min_time_begin_short_str||'_crttime on '||final_name||'_'||min_time_begin_short_str||' (rksj);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_alarm_source_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jjlybh) WHERE jjlybh IS NOT NULL;';
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_pjdb分表初始化-----------------------------------------------------------
DROP FUNCTION IF exists init_shard_icc_t_pjdb();
CREATE OR REPLACE FUNCTION init_shard_icc_t_pjdb()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	dt_data record;
	dt_data_str text;
	dt_jjsj timestamp;
	back_table_name text := 'icc_t_pjdb_bk';
	final_name text := 'icc_t_pjdb';
	index_name text := 'idx_t_pjdb';
	submeter_key text := 'jjsj';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			xzqhdm character varying(12),
			pjdbh character varying(30) NOT NULL,
			jjdbh character varying(30),
			pjdwdm character varying(12),
			pjybh character varying(30),
			pjyxm character varying(50),
			pjlyh character varying(255),
			pjyj character varying(4000),
			cjdwdm character varying(12),
			pjsj timestamp(6) without time zone,
			xtzdddsj timestamp(6) without time zone,
			pdqssj timestamp(6) without time zone,
			qsryxm character varying(50),
			cdry character varying(200), 
			cdcl character varying(500),
			cdct character varying(500),
			jqclztdm character varying(2),
			gxsj timestamp(6) without time zone,
			dqywzt character varying(32),
			cszt character varying(32),
			pjdwmc character varying(128),
			pjdwdmbs character varying(200),
			cjdxid character varying(64),
			cjdxdm character varying(200),
			cjdxmc character varying(128),
			cjdxlx character varying(32),
			cjdwdmbs character varying(200),
			KJYJBH character varying(64),
			qsrydm character varying(64), 
			tdsj timestamp(6) without time zone,
			sfzddpcj smallint NOT NULL,
			cxyy character varying(512), 
			cjry character varying(64),
			gxry character varying(64),
			sfja smallint NOT NULL,
			jasm character varying(1000),
			sjpjdbh character varying(64),
			gzldm character varying(64),
			CJZDBS character varying(50),
			GXZDBS character varying(50),
			BJPJDBM character varying(32),
			BJJJDBM character varying(32),
			XZQHMC character varying(100),
			CJDWMC character varying(100),
			SHLDDWDM character varying(6),
			QSRYBM character varying(30),
			RKSJ timestamp(6) without time zone,
			JASJ timestamp(6) without time zone,
			WBCSZT character varying(32),
			JYLXBM character varying(20),
			CJDWJC character varying(100),
			PJDWJC character varying(100),
			jjsj timestamp(6) without time zone
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
		strSQL :=  'COMMENT ON COLUMN public.'||final_name||'.xzqhdm IS ''派警单位行政区划'';
					COMMENT ON COLUMN public.'||final_name||'.pjdbh IS ''派警单编号'';
					COMMENT ON COLUMN public.'||final_name||'.jjdbh IS ''接警单编号'';
					COMMENT ON COLUMN public.'||final_name||'.pjdwdm IS ''下达派警指令的单位机构代码'';
					COMMENT ON COLUMN public.'||final_name||'.pjybh IS ''派警员编号，用来标识派警员身份'';
					COMMENT ON COLUMN public.'||final_name||'.pjyxm IS ''派警员姓名'';
					COMMENT ON COLUMN public.'||final_name||'.pjlyh IS ''派警录音号，派警时由录音系统自动产生，接处警系统自动关联'';
					COMMENT ON COLUMN public.'||final_name||'.pjyj IS ''派警意见的详细文字描述'';
					COMMENT ON COLUMN public.'||final_name||'.cjdwdm IS ''出警单位机构代码'';
					COMMENT ON COLUMN public.'||final_name||'.pjsj IS ''派警员下达派警单的时间'';
					COMMENT ON COLUMN public.'||final_name||'.jjsj IS ''接警单表中接警时间'';
					COMMENT ON COLUMN public.'||final_name||'.xtzdddsj IS ''出警单位的应用系统自动接收到的时间'';
					COMMENT ON COLUMN public.'||final_name||'.pdqssj IS ''网络下达派警单后，出警单位手工确认接到该派警单的时间'';
					COMMENT ON COLUMN public.'||final_name||'.qsryxm IS ''签收警单人员姓名'';
					COMMENT ON COLUMN public.'||final_name||'.cdry IS ''记录出警人员姓名等详细信息'';
					COMMENT ON COLUMN public.'||final_name||'.cdcl IS ''记录出警车辆的详细信息'';
					COMMENT ON COLUMN public.'||final_name||'.cdct IS ''记录出警船艇的详细信息'';
					COMMENT ON COLUMN public.'||final_name||'.jqclztdm IS ''记录该条警情目前的业务状态'';
					COMMENT ON COLUMN public.'||final_name||'.rksj IS ''记录创建的时间'';
					COMMENT ON COLUMN public.'||final_name||'.gxsj IS ''更新时间'';
					COMMENT ON COLUMN public.'||final_name||'.pjdwmc IS ''派警单位名称'';
					COMMENT ON COLUMN public.'||final_name||'.pjdwdmbs IS ''派警单位短码'';
					COMMENT ON COLUMN public.'||final_name||'.cjdwmc IS ''处警单位名称'';
					COMMENT ON COLUMN public.'||final_name||'.cjdwdmbs IS ''处警单位短码'';
					COMMENT ON COLUMN public.'||final_name||'.cjdxlx IS ''处警对象类型（单位、警员、车辆）'';
					COMMENT ON COLUMN public.'||final_name||'.cjdxmc IS ''处警对象名称'';
					COMMENT ON COLUMN public.'||final_name||'.cjdxid IS ''处警对象编码'';
					COMMENT ON COLUMN public.'||final_name||'.dqywzt IS ''vcs内部业务状态'';
					COMMENT ON COLUMN public.'||final_name||'.qsrydm IS ''签收人员代码(坐席编号)'';
					COMMENT ON COLUMN public.'||final_name||'.tdsj IS ''退单申请时间'';
					COMMENT ON COLUMN public.'||final_name||'.cxyy IS ''取消原因'';
					COMMENT ON COLUMN public.'||final_name||'.sfzddpcj IS ''是否自动调派任务创建'';
					COMMENT ON COLUMN public.'||final_name||'.cjry IS ''创建人'';
					COMMENT ON COLUMN public.'||final_name||'.gxry IS ''更新人'';
					COMMENT ON COLUMN public.'||final_name||'.cszt IS ''超时状态'';
					COMMENT ON COLUMN public.'||final_name||'.BJPJDBM IS ''部级派警单编码'';
					COMMENT ON COLUMN public.'||final_name||'.BJJJDBM IS ''部级接警单编码'';
					COMMENT ON COLUMN public.'||final_name||'.XZQHMC IS ''行政区划名称'';
					COMMENT ON COLUMN public.'||final_name||'.SHLDDWDM IS ''社会联动单位代码'';
					COMMENT ON COLUMN public.'||final_name||'.QSRYBM IS ''签收人员编码'';
					COMMENT ON COLUMN public.'||final_name||'.JASJ IS ''结案时间'';
					COMMENT ON COLUMN public.'||final_name||'.WBCSZT IS ''完毕超时状态'';
					COMMENT ON COLUMN public.'||final_name||'.JYLXBM IS ''警员类型编码'';
					COMMENT ON COLUMN public.'||final_name||'.CJDWJC IS ''出警单位代码对应简称'';
					COMMENT ON COLUMN public.'||final_name||'.PJDWJC IS ''派警单位简称'';
					COMMENT ON COLUMN public.'||final_name||'.CJZDBS IS ''创建终端标示'';
					COMMENT ON COLUMN public.'||final_name||'.GXZDBS IS ''更新终端标示'';';
	EXECUTE strSQL;

	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (pjdbh); 
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_threeinone ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jjdbh, pdqssj, jqclztdm);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_threeinone2 ON '||final_name||'_'||min_time_begin_short_str||' USING btree (cjdwdm, pdqssj, jqclztdm);
			      CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_time_submit ON '||final_name||'_'||min_time_begin_short_str||' USING brin (pjsj) WITH (pages_per_range=1);
				  create index '||index_name||'_'||min_time_begin_short_str||'_crttime on '||final_name||'_'||min_time_begin_short_str||'(rksj);
				  create index '||index_name||'_'||min_time_begin_short_str||'_uptime on '||final_name||'_'||min_time_begin_short_str||'(gxsj);';		 
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_alarm_log分表初始化-----------------------------------------------------------
DROP FUNCTION IF exists init_shard_icc_t_alarm_log();
CREATE OR REPLACE FUNCTION init_shard_icc_t_alarm_log()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	dt_data record;
	dt_data_str text;
	dt_jjsj timestamp;
	back_table_name text := 'icc_t_alarm_log_bk';
	final_name text := 'icc_t_alarm_log';
	index_name text := 'idx_t_alarm_log';
	submeter_key text := 'jjsj';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			id character varying(64) NOT NULL,
			jjdbh character varying(64), 
			pjdbh character varying(64), 
			fkdbh character varying(64),
			operate character varying(64),
			operatorcontent text,
			fromtype character varying(64),
			fromcode character varying(64),
			fromname character varying(200),
			fromorgcode character varying(64),
			fromorgname character varying(200),
			fromorgidentifier character varying(200),
			targettype character varying(64),
			targetcode character varying(64),
			targetname character varying(200),
			targetorgcode character varying(64),
			targetorgname character varying(200),
			targetorgidentifier character varying(200),
			operatetime timestamp(6) without time zone,
			sourcename character varying(64),
			operateattachdesc text,
			description text,
			createuser character varying(64), 
			createorg character varying(64),
			jjsj timestamp(6) without time zone
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;

	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (id); 
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_alarmid ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jjdbh);';		 
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_callevent分表初始化--------------------------------------------------------------
DROP FUNCTION IF exists isnumeric();
CREATE OR REPLACE FUNCTION isnumeric (txtStr VARCHAR) 
RETURNS BOOLEAN AS $$
BEGIN
RETURN txtStr ~ '^([0-9]+[.]?[0-9]*|[.][0-9]+)$' ;
END; 
$$ LANGUAGE 'plpgsql';

DROP FUNCTION IF exists init_shard_icc_t_callevent();
CREATE OR REPLACE FUNCTION init_shard_icc_t_callevent()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	dt_data record;
	dt_data_str text;
	dt_jjsj timestamp;
	dt_incomming_len interval;
	dt_ring_len interval;
	dt_talk_len interval;
	dt_create_time_start timestamp;
	dt_create_time_end timestamp;
	dt_str_jjsj text;
	dt_year text;
	dt_month text;
	dt_day text;
	dt_hour text;
	dt_minute text;
	dt_sec text;
	back_table_name text := 'icc_t_callevent_bk';
	final_name text := 'icc_t_callevent';
	index_name text := 'idx_t_callevent';
	submeter_key text := 'create_time';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;
	
	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			callref_id character varying(64) NOT NULL,
		    acd character varying(64),
		    caller_id character varying(64),
		    called_id character varying(64),
		    call_direction character varying(64),
		    talk_time timestamp(6) without time zone,
		    hangup_time timestamp(6) without time zone,
		    hangup_type character varying(64),
		    is_blackcall character varying(64),
		    incoming_time timestamp(6) without time zone,
		    dial_time timestamp(6) without time zone,
		    ring_time timestamp(6) without time zone,
		    ringback_time timestamp(6) without time zone,
		    is_callback character varying(64),
			switch_type character varying(32),
			original_acd character varying(64),
			acd_dept character varying(256),
			rel_alarm_id character varying(256),
			acd_relation_dept character varying(64),
			acd_relation_dept_path character varying(2048),
			acd_relation_dept_name character varying(256),
			caller_relation_dept character varying(64),
			caller_relation_dept_path character varying(2048),
			caller_relation_dept_name character varying(256),
			called_relation_dept character varying(64),
			called_relation_dept_path character varying(2048),
			called_relation_dept_name character varying(256),
			caller_type character varying(64),
			called_type character varying(64),
			receiver_code character varying(64),
			receiver_name character varying(128),
			accept_state integer,
			update_time timestamp(6) without time zone,
			flag character varying(64),
			acd_relation_dept_identify character varying(2048),
			caller_relation_dept_identify character varying(2048),
			called_relation_dept_identify character varying(2048),
			rel_alarm_type character varying(64),
			create_time timestamp(6) without time zone,
			incoming_len interval,
			ring_len interval,
			talk_len interval,
			record_file_id text
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (callref_id); 
				  CREATE INDEX idx_t_callevent_'||min_time_begin_short_str||'_callerd_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (caller_id, called_id);
				  CREATE INDEX idx_t_callevent_'||min_time_begin_short_str||'_ring_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (ring_time) WITH (pages_per_range=1);
				  CREATE INDEX idx_t_callevent_'||min_time_begin_short_str||'_ringback_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (ringback_time) WITH (pages_per_range=1);
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_call_direction ON '||final_name||'_'||min_time_begin_short_str||' USING btree (call_direction);
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_rel_alarm_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (rel_alarm_id);
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_incomng_time ON '||final_name||'_'||min_time_begin_short_str||' USING btree (incoming_time);
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_hangup_type ON '||final_name||'_'||min_time_begin_short_str||' USING btree (hangup_type);
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_record_file_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (record_file_id);';
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;

	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_sub_callevent分表初始化--------------------------------------------------------------
DROP FUNCTION IF exists isnumeric();
CREATE OR REPLACE FUNCTION isnumeric (txtStr VARCHAR) 
RETURNS BOOLEAN AS $$
BEGIN
RETURN txtStr ~ '^([0-9]+[.]?[0-9]*|[.][0-9]+)$' ;
END; 
$$ LANGUAGE 'plpgsql';

DROP FUNCTION IF exists init_shard_icc_t_sub_callevent();
CREATE OR REPLACE FUNCTION init_shard_icc_t_sub_callevent()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	dt_data record;
	dt_data_str text;
	dt_jjsj timestamp;
	dt_str_jjsj text;
	dt_year text;
	dt_month text;
	dt_day text;
	dt_hour text;
	dt_minute text;
	dt_sec text;
	back_table_name text := 'icc_t_sub_callevent_bk';
	final_name text := 'icc_t_sub_callevent';
	index_name text := 'idx_t_sub_callevent';
	submeter_key text := 'create_time';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;
	
	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			guid character varying(64) NOT NULL,
			callref_id character varying(64),
			eventcall_state character varying(64),
			state_time timestamp(6) without time zone,
			sponsor character varying(64),
			receiver character varying,
			create_time timestamp(6) without time zone
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (guid); 
				  CREATE INDEX idx_t_sub_callevent_'||min_time_begin_short_str||'_callref_id ON '||final_name||'_'||min_time_begin_short_str||' USING brin (callref_id);';		 
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_fkdb分表初始化-----------------------------------------------------------
DROP FUNCTION IF exists init_shard_icc_t_fkdb();
CREATE OR REPLACE FUNCTION init_shard_icc_t_fkdb()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	dt_data record;
	dt_data_str text;
	dt_jjsj timestamp;
	back_table_name text := 'icc_t_fkdb_bk';
	final_name text := 'icc_t_fkdb';
	index_name text := 'idx_t_fkdb';
	submeter_key text := 'jjsj';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			xzqhdm character varying(12),
			fkdbh character varying(30),
			jjdbh character varying(30),
			pjdbh character varying(30),
			fklyh character varying(255),
			fkdwdm character varying(12),
			fkybh character varying(64),
			fkyxm character varying(50),
			fksj timestamp(6) without time zone,
			cjsj01 timestamp(6) without time zone,
			ddxcsj timestamp(6) without time zone,
			xcclwbsj timestamp(6) without time zone,
			jqlbdm character varying(8),
			jqlxdm character varying(8),
			jqxldm character varying(8),
			jqzldm character varying(8),
			jqfssj timestamp(6) without time zone,
			bjdz character varying(200),
			jqdz character varying(200),
			fkdwxzb numeric,
			fkdwyzb numeric,
			cjczqk character varying(4000),
			cdcc smallint default 0,
			cdrc smallint default 0,
			cdct character varying(500),
			jzrs smallint default 0,
			jzrssm character varying(500),
			ssrs smallint default 0,
			ssrssm character varying(500),
			swrs smallint default 0,
			swrssm character varying(500),
			jqcljgdm character varying(64),
			jqcljgsm character varying(4000),
			tqqkdm character varying(64),
			ssqkms character varying(1000),
			zhrs smallint default 0,
			sars smallint default 0,
			tprs smallint default 0,
			jtsgxtdm character varying(64),
			sfzzwxp character varying(64),
			jtsgccyydm character varying(64),
			njddm character varying(64),
			lmzkdm character varying(64),
			shjdcs smallint default 0,
			shfjdcs smallint default 0,
			dllxdm character varying(64),
			jqclztdm character varying(2),
			cjsj timestamp(6) without time zone,
			gxsj timestamp(6) without time zone,
			jdxz character varying(64),
			sdcs character varying(64),
			sfqz character varying(64),
			yjdwdm character varying(12),
			yjdwlxr character varying(32),
			yjdwfkqk character varying(4000),
			yjdwfkqksm character varying(1000),
			ajbh character varying(64),
			nrbq character varying(800),
			jqbwdm character varying(32),
			fklx smallint,
			jqfsdy character varying(200),
			forcedm character varying(200),
			forcetype character varying(8),
			dqywzt character varying(32),
			fkdwdmbs character varying(200),
			fkdwmc character varying(128),
			livestatus character varying(64),
			tjdwdmbs character varying(64),
			wxbs character varying(64),
			yjdwmc character varying(128),
			yjdwdmbs character varying(200),
			tzhgxdwdm character varying(200),
			tzhgxdwmc character varying(128),
			tzhgxdwdmbs character varying(200),
			yjdwlxrdm character varying(200),
			xzqhmc character varying(100),
			gcfkbs boolean,
			jjsj timestamp(6) without time zone
			)partition by range('||submeter_key||');';
	EXECUTE strSQL;
	
	COMMENT ON COLUMN public.icc_t_fkdb.xzqhdm
    IS '行政区划代码';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdbh
	    IS '反馈单编号';
	COMMENT ON COLUMN public.icc_t_fkdb.jjdbh
	    IS '接警单编号';
	COMMENT ON COLUMN public.icc_t_fkdb.pjdbh
	    IS '派警单编号';
	COMMENT ON COLUMN public.icc_t_fkdb.fklyh
	    IS '反馈录音号';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwdm
	    IS '反馈单位代码';
	COMMENT ON COLUMN public.icc_t_fkdb.fkybh
	    IS '反馈员编号';
	COMMENT ON COLUMN public.icc_t_fkdb.fkyxm
	    IS '反馈员姓名';
	COMMENT ON COLUMN public.icc_t_fkdb.fksj
	    IS '反馈时间';
	COMMENT ON COLUMN public.icc_t_fkdb.cjsj01
	    IS '实际出警时间';
	COMMENT ON COLUMN public.icc_t_fkdb.ddxcsj
	    IS '到达现场时间';
	COMMENT ON COLUMN public.icc_t_fkdb.xcclwbsj
	    IS '现场处理完毕时间';
	COMMENT ON COLUMN public.icc_t_fkdb.jqlbdm
	    IS '警情类别代码';
	COMMENT ON COLUMN public.icc_t_fkdb.jqlxdm
	    IS '警情类型代码';
	COMMENT ON COLUMN public.icc_t_fkdb.jqxldm
	    IS '警情细类代码';
	COMMENT ON COLUMN public.icc_t_fkdb.jqzldm
	    IS '警情子类代码';
	COMMENT ON COLUMN public.icc_t_fkdb.jqfssj
	    IS '警情发生时间';
	COMMENT ON COLUMN public.icc_t_fkdb.bjdz
	    IS '报警地址';
	COMMENT ON COLUMN public.icc_t_fkdb.jqdz
	    IS '警情地址';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwxzb
	    IS '反馈定位X坐标(处置现场位置)';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwyzb
	    IS '反馈定位Y坐标(处置现场位置)';
	COMMENT ON COLUMN public.icc_t_fkdb.cjczqk
	    IS '出警处置情况';
	COMMENT ON COLUMN public.icc_t_fkdb.cdcc
	    IS '出动车次';
	COMMENT ON COLUMN public.icc_t_fkdb.cdrc
	    IS '出动人次';
	COMMENT ON COLUMN public.icc_t_fkdb.cdct
	    IS '出动船艇';
	COMMENT ON COLUMN public.icc_t_fkdb.jzrs
	    IS '救助人数';
	COMMENT ON COLUMN public.icc_t_fkdb.jzrssm
	    IS '救助人数说明';
	COMMENT ON COLUMN public.icc_t_fkdb.ssrs
	    IS '受伤人数';
	COMMENT ON COLUMN public.icc_t_fkdb.ssrssm
	    IS '受伤人数说明';
	COMMENT ON COLUMN public.icc_t_fkdb.swrs
	    IS '死亡人数';
	COMMENT ON COLUMN public.icc_t_fkdb.swrssm
	    IS '死亡人数说明';
	COMMENT ON COLUMN public.icc_t_fkdb.jqcljgdm
	    IS '警情处理结果代码';
	COMMENT ON COLUMN public.icc_t_fkdb.jqcljgsm
	    IS '警情处理结果说明';
	COMMENT ON COLUMN public.icc_t_fkdb.tqqkdm
	    IS '天气情况代码';
	COMMENT ON COLUMN public.icc_t_fkdb.ssqkms
	    IS '损失情况描述';
	COMMENT ON COLUMN public.icc_t_fkdb.zhrs
	    IS '抓获人数';
	COMMENT ON COLUMN public.icc_t_fkdb.sars
	    IS '涉案人数';
	COMMENT ON COLUMN public.icc_t_fkdb.tprs
	    IS '逃跑人数';
	COMMENT ON COLUMN public.icc_t_fkdb.jtsgxtdm
	    IS '交通事故形态代码';
	COMMENT ON COLUMN public.icc_t_fkdb.sfzzwxp
	    IS '是否装载危险品';
	COMMENT ON COLUMN public.icc_t_fkdb.jtsgccyydm
	    IS '交通事故初查原因代码';
	COMMENT ON COLUMN public.icc_t_fkdb.njddm
	    IS '能见度代码';
	COMMENT ON COLUMN public.icc_t_fkdb.lmzkdm
	    IS '路面状况代码';
	COMMENT ON COLUMN public.icc_t_fkdb.shjdcs
	    IS '损坏机动车数';
	COMMENT ON COLUMN public.icc_t_fkdb.shfjdcs
	    IS '损坏非机动车数';
	COMMENT ON COLUMN public.icc_t_fkdb.dllxdm
	    IS '道路类型代码';
	COMMENT ON COLUMN public.icc_t_fkdb.jqclztdm
	    IS '警情处理状态代码';
	COMMENT ON COLUMN public.icc_t_fkdb.cjsj
	    IS '创建时间';
	COMMENT ON COLUMN public.icc_t_fkdb.gxsj
	    IS '更新时间';
	COMMENT ON COLUMN public.icc_t_fkdb.jdxz
	    IS '乡镇(街道)';
	COMMENT ON COLUMN public.icc_t_fkdb.sdcs
	    IS '属地村社';
	COMMENT ON COLUMN public.icc_t_fkdb.sfqz
	    IS '是否取证';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwdm
	    IS '移交单位代码';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwlxr
	    IS '移交单位联系人名称';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwfkqk
	    IS '移交单位反馈情况';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwfkqksm
	    IS '移交单位反馈情况说明';
	COMMENT ON COLUMN public.icc_t_fkdb.ajbh
	    IS '案件编号';
	COMMENT ON COLUMN public.icc_t_fkdb.nrbq
	    IS '警情标签';
	COMMENT ON COLUMN public.icc_t_fkdb.jqbwdm
	    IS '警情部位代码';
	COMMENT ON COLUMN public.icc_t_fkdb.jqfsdy
	    IS '警情发生地域';
	COMMENT ON COLUMN public.icc_t_fkdb.forcedm
	    IS '警力代码';
	COMMENT ON COLUMN public.icc_t_fkdb.forcetype
	    IS '警力类型';
	COMMENT ON COLUMN public.icc_t_fkdb.dqywzt
	    IS '当前业务状态';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwdmbs
	    IS '反馈单位短码';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwmc
	    IS '反馈单位名称';
	COMMENT ON COLUMN public.icc_t_fkdb.livestatus
	    IS '现场状态';
	COMMENT ON COLUMN public.icc_t_fkdb.tjdwdmbs
	    IS '提交单位短码';
	COMMENT ON COLUMN public.icc_t_fkdb.wxbs
	    IS '是否无效警情标识';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwmc
	    IS '移交单位名称';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwdmbs
	    IS '移交单位短码';
	COMMENT ON COLUMN public.icc_t_fkdb.tzhgxdwdm
	    IS '调整后管辖单位代码';
	COMMENT ON COLUMN public.icc_t_fkdb.tzhgxdwmc
	    IS '调整后管辖单位名称';
	COMMENT ON COLUMN public.icc_t_fkdb.tzhgxdwdmbs
	    IS '调整后管辖单位短码';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwlxrdm
	    IS '移交单位联系人代码';
	COMMENT ON COLUMN public.icc_t_fkdb.gcfkbs
	    IS '过程反馈标识';
	COMMENT ON COLUMN public.icc_t_fkdb.xzqhmc
	    IS '行政区划代码对应名称';

	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (pjdbh, fkdbh); 
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_five_in_one ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jjdbh,jqlbdm,jqlxdm,jqxldm,jqzldm);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (fkdbh);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_update_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (gxsj) WITH (pages_per_range=1);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_crttime ON '||final_name||'_'||min_time_begin_short_str||'(cjsj);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_uptime ON '||final_name||'_'||min_time_begin_short_str||'(gxsj);';		 
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;

	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

---------------------------------------------------------录音文件记录表分表初始化--------------------------------------------------------------
DROP FUNCTION IF exists init_shard_icc_t_record_files_item();
CREATE OR REPLACE FUNCTION init_shard_icc_t_record_files_item()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	final_name text := 'icc_t_record_files_item';
	index_name text := 'idx_t_';
	submeter_key text := 'start_time';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			guid character varying(64) NOT NULL,
			record_file_id character varying(64),
			callref_id character varying(64) ,
			record_file_name character varying(256) NOT NULL,
			record_file_url character varying(512),
			agent_phone_ip character varying(64),
			agent_phone_no character varying(64),
			caller_no character varying(64),
			called_no character varying(64),
			cti_refreshed boolean,
			start_time timestamp(6) without time zone,
			stop_time timestamp(6) without time zone
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
		strSQL :=  'COMMENT ON TABLE public.'||final_name||' IS ''存储文件记录表'';
		            COMMENT ON COLUMN public.'||final_name||'.guid IS ''主键'';
					COMMENT ON COLUMN public.'||final_name||'.record_file_id IS ''存储文件ID'';
					COMMENT ON COLUMN public.'||final_name||'.callref_id IS ''关联的话务ID'';
					COMMENT ON COLUMN public.'||final_name||'.record_file_name IS ''存储文件名称'';
					COMMENT ON COLUMN public.'||final_name||'.record_file_url IS ''文件链接'';
					COMMENT ON COLUMN public.'||final_name||'.agent_phone_ip IS ''坐席话机IP'';
					COMMENT ON COLUMN public.'||final_name||'.agent_phone_no IS ''坐席话机号码'';
					COMMENT ON COLUMN public.'||final_name||'.caller_no IS ''主叫号码'';
					COMMENT ON COLUMN public.'||final_name||'.called_no IS ''被叫号码'';
					COMMENT ON COLUMN public.'||final_name||'.cti_refreshed IS ''是否已经刷新CTI话务记录表'';
					COMMENT ON COLUMN public.'||final_name||'.start_time IS ''呼叫开始时间'';
					COMMENT ON COLUMN public.'||final_name||'.stop_time IS ''呼叫结束时间'';';
	EXECUTE strSQL;

	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (guid); 
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_record_file_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (record_file_id);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_record_file_name ON '||final_name||'_'||min_time_begin_short_str||' USING btree (record_file_name);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_agent_phone_no ON '||final_name||'_'||min_time_begin_short_str||' USING btree (agent_phone_no);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_caller_no ON '||final_name||'_'||min_time_begin_short_str||' USING btree (caller_no);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_called_no ON '||final_name||'_'||min_time_begin_short_str||' USING btree (called_no);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_start_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (start_time) WITH (pages_per_range=1);
			      CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_stop_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (stop_time) WITH (pages_per_range=1);';
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_autourge分表初始化-----------------------------------------------------------
DROP FUNCTION IF exists shard_icc_t_autourge();
CREATE OR REPLACE FUNCTION shard_icc_t_autourge()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	dt_data record;
	back_table_name text := 'icc_t_autourge_bk';
	final_name text := 'icc_t_autourge';
	index_name text := 'idx_t_autourge';
	submeter_key text := 'urgestarttime';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			  "alarmid" varchar(50) COLLATE "pg_catalog"."default",
			  "urgeid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
			  "timerid" varchar(50) COLLATE "pg_catalog"."default",
			  "department" varchar(50) COLLATE "pg_catalog"."default",
			  "voicetype" varchar(50) COLLATE "pg_catalog"."default",
			  "voiceid" varchar(50) COLLATE "pg_catalog"."default",
			  "ttscontent" varchar(50) COLLATE "pg_catalog"."default",
			  "smscontent" varchar(50) COLLATE "pg_catalog"."default",
			  "urgecalled" varchar(50) COLLATE "pg_catalog"."default",
			  "urgelevel" varchar(50) COLLATE "pg_catalog"."default",
			  "urgecount" varchar(50) COLLATE "pg_catalog"."default",
			  "callcount" integer default 0,
			  "callokcount" integer default 0,
			  "skipcount" integer default 0,
			  "urgestarttime" timestamp(6) without time zone,
			  "urgeinterval" varchar(50) COLLATE "pg_catalog"."default",
			  "createuser" varchar(50) COLLATE "pg_catalog"."default",
			  "updateuser" varchar(50) COLLATE "pg_catalog"."default",
			  "updatetime" timestamp(6) without time zone,
			  "seatno" varchar(50) COLLATE "pg_catalog"."default",
			  "remarks" varchar(50) COLLATE "pg_catalog"."default",
			  "urgestate" varchar(50) COLLATE "pg_catalog"."default",
			  "is_confirm" varchar(50) COLLATE "pg_catalog"."default"
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
		strSQL := 'COMMENT ON COLUMN public.'||final_name||'.alarmid
				IS ''警情ID'';

				COMMENT ON COLUMN public.'||final_name||'.urgeid
					IS ''催办ID'';
					
				COMMENT ON COLUMN public.'||final_name||'.timerid 
					IS ''催办定时器ID'';
					
				COMMENT ON COLUMN public.'||final_name||'.department 
					IS ''部门'';
					
				COMMENT ON COLUMN public.'||final_name||'.voicetype
					IS ''语音类型:0:语音文件播报,文件通过voice_id确定; 1:TTS文本播报催办; 2:短信催办'';
	
				COMMENT ON COLUMN public.'||final_name||'.voiceid
					IS ''语音文件ID'';
	
				COMMENT ON COLUMN public.'||final_name||'.ttscontent
					IS ''TTS文本内容'';
	
				COMMENT ON COLUMN public.'||final_name||'.smscontent
					IS ''短信催办内容'';
					
				COMMENT ON COLUMN public.'||final_name||'.urgecalled
					IS ''催办号码'';
					
				COMMENT ON COLUMN public.'||final_name||'.urgelevel
					IS ''催办优先级(1-4级)'';
	
				COMMENT ON COLUMN public.'||final_name||'.urgecount
					IS ''催办总次数'';
					
				COMMENT ON COLUMN public.'||final_name||'.callcount
					IS ''催办呼叫次数'';
					
				COMMENT ON COLUMN public.'||final_name||'.skipcount
					IS ''催办跳过次数'';
					
				COMMENT ON COLUMN public.'||final_name||'.callokcount
					IS ''催办呼叫成功次数'';
					
				COMMENT ON COLUMN public.'||final_name||'.urgestarttime
					IS ''催办启动时间'';
	
				COMMENT ON COLUMN public.'||final_name||'.urgeinterval
					IS ''催办间隔时间(单位s)'';
	
				COMMENT ON COLUMN public.'||final_name||'.createuser
					IS ''创建用户'';
						
				COMMENT ON COLUMN public.'||final_name||'.updateuser
					IS ''更新用户'';
					
				COMMENT ON COLUMN public.'||final_name||'.updatetime
					IS ''更新时间'';
					
				COMMENT ON COLUMN public.'||final_name||'.seatno
					IS ''坐席号'';
					
				COMMENT ON COLUMN public.'||final_name||'.remarks
					IS ''备注信息'';
					
				COMMENT ON COLUMN public.'||final_name||'.urgestate
					IS ''催办状态:0:未开始, 1:催办中, 2:对方已确认, 3:催办完成, 4:催办停止, 5:催办失败中止'';
					
				COMMENT ON TABLE public.'||final_name||'
					IS ''自动催办基本信息表'';
					
				COMMENT ON COLUMN public.'||final_name||'.is_confirm
					IS ''是否需要确认'';';
	EXECUTE strSQL;

	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (urgeid);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_timerid ON '||final_name||'_'||min_time_begin_short_str||' USING btree (timerid);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_voicetype ON '||final_name||'_'||min_time_begin_short_str||' USING btree (voicetype);
				  create index '||index_name||'_'||min_time_begin_short_str||'_urge_start_time on '||final_name||'_'||min_time_begin_short_str||' USING brin (urgestarttime) WITH (pages_per_range=1);';
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_autourge_call分表初始化-----------------------------------------------------------
DROP FUNCTION IF exists shard_icc_t_autourge_call();
CREATE OR REPLACE FUNCTION shard_icc_t_autourge_call()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	dt_data record;
	back_table_name text := 'icc_t_autourge_call_bk';
	final_name text := 'icc_t_autourge_call';
	index_name text := 'idx_t_autourge_call';
	submeter_key text := 'createtime';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			  "guid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
			  "timerid" varchar(50) COLLATE "pg_catalog"."default",
			  "callid" varchar(50) COLLATE "pg_catalog"."default",
			  "makecalltime" timestamp(6) without time zone,
			  "ringtime" timestamp(6) without time zone,
			  "answertime" timestamp(6) without time zone,
			  "hanguptime" timestamp(6) without time zone,
			  "callresultcode" varchar(5) COLLATE "pg_catalog"."default",
			  "callresult" varchar(100) COLLATE "pg_catalog"."default",
			  "filename" varchar(50) COLLATE "pg_catalog"."default",
			  "createtime" timestamp(6) without time zone,
			  "updatetime" timestamp(6) without time zone,
			  "callconfirm" varchar(5) COLLATE "pg_catalog"."default"
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
		strSQL := 'COMMENT ON COLUMN public.'||final_name||'.guid
				IS ''guid'';

				COMMENT ON COLUMN public.'||final_name||'.timerid
					IS ''定时器ID'';
					
				COMMENT ON COLUMN public.'||final_name||'.callid 
					IS ''电话ID'';
					
				COMMENT ON COLUMN public.'||final_name||'.makecalltime 
					IS ''拨号时间'';
					
				COMMENT ON COLUMN public.'||final_name||'.ringtime
					IS ''振铃时间'';
	
				COMMENT ON COLUMN public.'||final_name||'.answertime
					IS ''应答时间'';
	
				COMMENT ON COLUMN public.'||final_name||'.hanguptime
					IS ''挂断时间'';
	
				COMMENT ON COLUMN public.'||final_name||'.callresultcode
					IS ''催办结果值'';
					
				COMMENT ON COLUMN public.'||final_name||'.callresult
					IS ''催办结果'';
					
				COMMENT ON COLUMN public.'||final_name||'.filename
					IS ''语音电话文件名'';
	
				COMMENT ON COLUMN public.'||final_name||'.createtime
					IS ''创建时间'';
					
				COMMENT ON COLUMN public.'||final_name||'.updatetime
					IS ''更新时间'';
					
				COMMENT ON COLUMN public.'||final_name||'.callconfirm
					IS ''是否确认'';
					
				COMMENT ON TABLE public.'||final_name||'
					IS ''自动催办通话表'';';
	EXECUTE strSQL;

	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (guid);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_timerid ON '||final_name||'_'||min_time_begin_short_str||' USING btree (timerid);
				  create index '||index_name||'_'||min_time_begin_short_str||'_callresult on '||final_name||'_'||min_time_begin_short_str||' USING btree (callresult);';
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_autourge_sms分表初始化-----------------------------------------------------------
DROP FUNCTION IF exists shard_icc_t_autourge_sms();
CREATE OR REPLACE FUNCTION shard_icc_t_autourge_sms()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time timestamp;
	min_time_begin timestamp;
	strSQL   text;
	time_difference interval;
	min_time_str text;
	min_time_begin_short_str text;
	min_time_begin_str text;
	dt_data record;
	back_table_name text := 'icc_t_autourge_sms_bk';
	final_name text := 'icc_t_autourge_sms';
	index_name text := 'idx_t_autourge_sms';
	submeter_key text := 'createtime';
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (select date_trunc('month',current_date)::DATE);
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			  "guid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
			  "smid" varchar(50) COLLATE "pg_catalog"."default",	
			  "timerid" varchar(50) COLLATE "pg_catalog"."default" NOT NULL,
			  "extension_no" varchar(50) COLLATE "pg_catalog"."default",
			  "sendtime" timestamp(6) without time zone,
			  "sendresult" varchar(50) COLLATE "pg_catalog"."default",
			  "sendresultstr" varchar(50) COLLATE "pg_catalog"."default",
			  "createtime" timestamp(6) without time zone,
			  "updatetime" timestamp(6) without time zone
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
		strSQL := 'COMMENT ON COLUMN public.'||final_name||'.guid
				IS ''guid'';

				COMMENT ON COLUMN public.'||final_name||'.smid
					IS ''rvs回传ID'';
					
				COMMENT ON COLUMN public.'||final_name||'.timerid 
					IS ''定时器ID'';
					
				COMMENT ON COLUMN public.'||final_name||'.extension_no 
					IS ''扩展号(为数字类型，长度不超过五位，短信回复时作为唯一性判断)'';
					
				COMMENT ON COLUMN public.'||final_name||'.sendtime
					IS ''发送时间'';
	
				COMMENT ON COLUMN public.'||final_name||'.sendresult
					IS ''发送结果'';
	
				COMMENT ON COLUMN public.'||final_name||'.sendresultstr
					IS ''具体提示信息'';
	
				COMMENT ON COLUMN public.'||final_name||'.createtime
					IS ''创建时间'';
					
				COMMENT ON COLUMN public.'||final_name||'.updatetime
					IS ''更新时间'';
				
				COMMENT ON TABLE public.'||final_name||'
					IS ''自动催办短信表'';';
	EXECUTE strSQL;

	WHILE time_difference > interval '0 day' LOOP
		min_time_str := to_char(min_time - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_str := to_char(min_time_begin, 'YYYY-MM-dd hh24:MI:ss');
		min_time_begin_short_str := to_char(min_time_begin, 'YYYYMM');
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (guid);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_timerid ON '||final_name||'_'||min_time_begin_short_str||' USING btree (timerid);
				  create index '||index_name||'_'||min_time_begin_short_str||'_smid on '||final_name||'_'||min_time_begin_short_str||' USING btree (smid) WHERE smid IS NOT NULL;';
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;

	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_jjdb新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shardtable_icc_t_jjdb();
CREATE OR REPLACE FUNCTION create_shardtable_icc_t_jjdb()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_jjdb';
	index_name text := 'idx_t_jjdb';
	submeter_key text := 'jjsj';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (jjdbh);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_addr ON '||final_name||'_'||min_time_begin_short_str||' USING gin (jqdz gin_trgm_ops);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_caller_no ON '||final_name||'_'||min_time_begin_short_str||' USING gin (bjdh gin_trgm_ops);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_content ON '||final_name||'_'||min_time_begin_short_str||' USING gin (bjnr gin_trgm_ops);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_id ON '||final_name||'_'||min_time_begin_short_str||' USING gin (jjdbh gin_trgm_ops);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_source_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jjlyh);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_ten_in_one ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jqclztdm, jqdjdm, lhlx, jqlbdm, jqlxdm, jqxldm, jjlx, gxdwdm, jjdwdm, jjybh);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (bjsj) WITH (pages_per_range=1);
				  create index '||index_name||'_'||min_time_begin_short_str||'_uptime on '||final_name||'_'||min_time_begin_short_str||' (gxsj);
				  create index '||index_name||'_'||min_time_begin_short_str||'_jjsjtime on '||final_name||'_'||min_time_begin_short_str||' USING brin (jjsj) WITH (pages_per_range=1);
				  create index '||index_name||'_'||min_time_begin_short_str||'_crttime on '||final_name||'_'||min_time_begin_short_str||' (rksj);
				  create index '||index_name||'_'||min_time_begin_short_str||'_glzjjdb on '||final_name||'_'||min_time_begin_short_str||' USING btree (glzjjdbh) WHERE glzjjdbh IS NOT NULL;';
				  
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;
-----------------------------------------------------------icc_t_pjdb新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shardtable_icc_t_pjdb();
CREATE OR REPLACE FUNCTION create_shardtable_icc_t_pjdb()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_pjdb';
	index_name text := 'idx_t_pjdb';
	submeter_key text := 'jjsj';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (pjdbh); 
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_threeinone ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jjdbh, pdqssj, jqclztdm);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_threeinone2 ON '||final_name||'_'||min_time_begin_short_str||' USING btree (cjdwdm, pdqssj, jqclztdm);
			      CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_time_submit ON '||final_name||'_'||min_time_begin_short_str||' USING brin (pjsj) WITH (pages_per_range=1);
				  create index '||index_name||'_'||min_time_begin_short_str||'_crttime on '||final_name||'_'||min_time_begin_short_str||'(rksj);
				  create index '||index_name||'_'||min_time_begin_short_str||'_uptime on '||final_name||'_'||min_time_begin_short_str||'(gxsj);';		 
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;
-----------------------------------------------------------icc_t_alarm_log新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shardtable_icc_t_alarm_log();
CREATE OR REPLACE FUNCTION create_shardtable_icc_t_alarm_log()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_alarm_log';
	index_name text := 'idx_t_alarm_log';
	submeter_key text := 'jjsj';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (id); 
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_alarmid ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jjdbh);';		 
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;
-----------------------------------------------------------icc_t_callevent新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shardtable_icc_t_callevent();
CREATE OR REPLACE FUNCTION create_shardtable_icc_t_callevent()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_callevent';
	index_name text := 'idx_t_callevent';
	submeter_key text := 'create_time';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (callref_id); 
				  CREATE INDEX idx_t_callevent_'||min_time_begin_short_str||'_callerd_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (caller_id, called_id);
				  CREATE INDEX idx_t_callevent_'||min_time_begin_short_str||'_ring_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (ring_time) WITH (pages_per_range=1);
				  CREATE INDEX idx_t_callevent_'||min_time_begin_short_str||'_ringback_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (ringback_time) WITH (pages_per_range=1);
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_call_direction ON '||final_name||'_'||min_time_begin_short_str||' USING btree (call_direction);
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_rel_alarm_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (rel_alarm_id);
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_hangup_type ON '||final_name||'_'||min_time_begin_short_str||' USING btree (hangup_type);
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_incomng_time ON '||final_name||'_'||min_time_begin_short_str||' USING btree (incoming_time);';
				  
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_sub_callevent新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shardtable_icc_t_sub_callevent();
CREATE OR REPLACE FUNCTION create_shardtable_icc_t_sub_callevent()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_sub_callevent';
	index_name text := 'idx_t_sub_callevent';
	submeter_key text := 'create_time';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (guid); 
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_callref_id ON '||final_name||'_'||min_time_begin_short_str||' USING brin (callref_id);';		 
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_fkdb新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shardtable_icc_t_fkdb();
CREATE OR REPLACE FUNCTION create_shardtable_icc_t_fkdb()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_fkdb';
	index_name text := 'idx_t_fkdb';
	submeter_key text := 'jjsj';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (pjdbh, fkdbh); 
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_five_in_one ON '||final_name||'_'||min_time_begin_short_str||' USING btree (jjdbh,jqlbdm,jqlxdm,jqxldm,jqzldm);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (fkdbh);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_update_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (gxsj) WITH (pages_per_range=1);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_crttime ON '||final_name||'_'||min_time_begin_short_str||'(cjsj);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_uptime ON '||final_name||'_'||min_time_begin_short_str||'(gxsj);';		 
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------录音文件记录表新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shardtable_icc_t_record_files_item();
CREATE OR REPLACE FUNCTION create_shardtable_icc_t_record_files_item()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_record_files_item';
	index_name text := 'icc_t_record_files_item';
	submeter_key text := 'start_time';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'CREATE INDEX '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' USING btree('||submeter_key||');';
		EXECUTE strSQL;
	    strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (guid); 
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_record_file_id ON '||final_name||'_'||min_time_begin_short_str||' USING btree (record_file_id);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_record_file_name ON '||final_name||'_'||min_time_begin_short_str||' USING btree (record_file_name);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_start_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (start_time) WITH (pages_per_range=1);
			      CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_stop_time ON '||final_name||'_'||min_time_begin_short_str||' USING brin (stop_time) WITH (pages_per_range=1);';  
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_autourge新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shard_icc_t_autourge();
CREATE OR REPLACE FUNCTION create_shard_icc_t_autourge()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_autourge';
	index_name text := 'idx_t_autourge';
	submeter_key text := 'urgestarttime';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (urgeid);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_timerid ON '||final_name||'_'||min_time_begin_short_str||' USING btree (timerid);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_voicetype ON '||final_name||'_'||min_time_begin_short_str||' USING btree (voicetype);
				  create index '||index_name||'_'||min_time_begin_short_str||'_urge_start_time on '||final_name||'_'||min_time_begin_short_str||' USING brin (urgestarttime) WITH (pages_per_range=1);';	 
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_autourge_call新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shard_icc_t_autourge_call();
CREATE OR REPLACE FUNCTION create_shard_icc_t_autourge_call()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_autourge_call';
	index_name text := 'idx_t_autourge_call';
	submeter_key text := 'createtime';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (guid);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_timerid ON '||final_name||'_'||min_time_begin_short_str||' USING btree (timerid);
				  create index '||index_name||'_'||min_time_begin_short_str||'_callresult on '||final_name||'_'||min_time_begin_short_str||' USING btree (callresult);';	 
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_autourge_sms新增下月分区表--------------------------------------------------------------
DROP FUNCTION IF exists create_shard_icc_t_autourge_sms();
CREATE OR REPLACE FUNCTION create_shard_icc_t_autourge_sms()
RETURNS text AS $sql_str$
DECLARE
	cur_time timestamp;
	min_time_begin_short_str text;
	min_time_begin_str text;
	min_time_str text;
	final_name text := 'icc_t_autourge_sms';
	index_name text := 'idx_t_autourge_sms';
	submeter_key text := 'createtime';
	is_created int;
	final_result text;
	full_name text;
	strSQL text;
begin
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	
	min_time_str := to_char(cur_time + interval '1 month' - interval '1 sec', 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_str := to_char(cur_time, 'YYYY-MM-dd hh24:MI:ss');
	min_time_begin_short_str := to_char(cur_time, 'YYYYMM');

	full_name := ''||final_name||'_'||min_time_begin_short_str||'';
	
	is_created := (select count(*) from information_schema.tables where table_schema='public' and table_type='BASE TABLE' and table_name= full_name);
	if is_created = 0 then
		strSQL := 'CREATE TABLE IF NOT EXISTS '||final_name||'_'||min_time_begin_short_str||' partition of '||final_name||' for values from ('''||min_time_begin_str||''') to ('''||min_time_str||''');';
		EXECUTE strSQL;
		strSQL := 'alter table '||final_name||'_'||min_time_begin_short_str||' replica identity full;';
		EXECUTE strSQL;
		strSQL := 'create index '||final_name||'_'||min_time_begin_short_str||'_sync_time on '||final_name||'_'||min_time_begin_short_str||' using btree('||submeter_key||');';
		EXECUTE strSQL;
		strSQL := 'ALTER TABLE ONLY '||final_name||'_'||min_time_begin_short_str||' ADD CONSTRAINT '||final_name||'_'||min_time_begin_short_str||'_pkey PRIMARY KEY (guid);
				  CREATE INDEX '||index_name||'_'||min_time_begin_short_str||'_timerid ON '||final_name||'_'||min_time_begin_short_str||' USING btree (timerid);
				  create index '||index_name||'_'||min_time_begin_short_str||'_smid on '||final_name||'_'||min_time_begin_short_str||' USING btree (smid) WHERE smid IS NOT NULL;';	 
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;
SET search_path = public, pg_catalog;

CREATE TYPE statistic_info AS (
	client_name character varying,
	client_id character varying,
	dept_name character varying,
	online_time timestamp without time zone,
	offline_time timestamp without time zone,
	online_time_len integer,
	offseat_time_len integer,
	release_call_num integer,
	total_call_num integer,
	count integer,
	alarm_num integer
);

CREATE TYPE statistic_result AS (
	guid character varying,
	client_name character varying,
	client_id character varying,
	dept_name character varying,
	online_time timestamp without time zone,
	offline_time timestamp without time zone,
	online_time_len integer,
	offseat_time_len integer,
	release_call_num integer,
	total_call_num integer,
	alarm_num integer,
	count integer
);

CREATE TYPE statistic_type_update AS (
	client_name character varying,
	client_id character varying,
	online_time timestamp without time zone,
	offline_time timestamp without time zone,
	online_time_length integer,
	offseat_time_length integer,
	release_callref_count integer,
	callref_total_count integer,
	alarm_count integer
);

CREATE FUNCTION count_estimate(query text) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
    rec   record;
    ROWS  INTEGER;
BEGIN
    FOR rec IN EXECUTE 'EXPLAIN ' || query LOOP
        ROWS := SUBSTRING(rec."QUERY PLAN" FROM ' rows=([[:digit:]]+)');
        EXIT WHEN ROWS IS NOT NULL;
    END LOOP;
 
    RETURN ROWS;
END
$$;

CREATE FUNCTION get_all_child_dept_code(parent_code character varying) RETURNS SETOF character varying
    LANGUAGE plpgsql
    AS $$declare
	result record;
 	begin
    
    return query(
        WITH RECURSIVE t (guid,parent_guid,code,name,path,depth) as(
        select a.guid,a.parent_guid,a.code,a.name,a.code as path,1 as depth
        from icc_t_dept a
        where code = parent_code
        union all select D.guid,D.parent_guid,D.code,D.name,t.path ||'/'|| D.code,t.depth + 1 as depth
        from icc_t_dept D join t on D.parent_guid = t.guid) 
        select code from t where path like '%'||parent_code||'%'
    );
    return;
   	end;
$$;

CREATE FUNCTION get_statistic_info(begin_time timestamp without time zone, end_time timestamp without time zone, page_size integer, page_index integer, user_dept character varying, client_name character varying, client_id character varying) RETURNS SETOF statistic_info
    LANGUAGE plpgsql
    AS $$
declare rec statistic_info%rowtype;
	query text;
BEGIN
  query = 'SELECT	A.client_name,A.client_id,B.name AS dept_name,
				A.on_time AS online_time,
				A.off_time AS offline_time,
				ROUND(EXTRACT ( EPOCH FROM CASE WHEN A.off_time IS NULL THEN now() ELSE A.off_time END - A.on_time )) AS online_time_len,
				ROUND(EXTRACT(EPOCH FROM (SELECT LEAST(A.off_time,NOW())-A.on_time-SUM(least(B.logout_time,LEAST(A.off_time,now())) - B.login_time) FROM icc_t_client_in_out B where B.login_time BETWEEN A.on_time AND CASE WHEN A.off_time is null THEN now() ELSE A.off_time END AND B.client_id=A.client_id AND B.client_name = A.client_name ))) AS offseat_time_len,
				COUNT (DISTINCT CASE WHEN C.hangup_type = ''release'' THEN C.callref_id ELSE NULL END) AS release_call_num,
				COUNT(DISTINCT C.callref_id ) AS total_call_num,
				COUNT ( 1 ) OVER () AS COUNT
	FROM
				icc_t_client_on_off A LEFT JOIN icc_t_callevent C ON ((C.called_id = A.client_id AND C.call_direction = ''in'') OR (C.caller_id = A.client_id AND C.call_direction=''out'')) AND C.ring_time BETWEEN A.on_time AND CASE WHEN A.off_time IS NULL THEN now() ELSE A.off_time END
				LEFT JOIN icc_t_dept B ON B.code = A.dept_code
         	WHERE A.on_time >= '''||begin_time ||''' and A.on_time <= '''||end_time ||''' and (A.off_time >= A.on_time or A.off_time is null) and A.dept_code = any (SELECT dept.code FROM icc_view_dept_recursive dept WHERE dept."path" like ''%'||user_dept||'%'')';
	
	if client_name != '' THEN
		query = query || ' AND A.client_name = '''|| client_name ||'''';
	end if;
	
	if client_id != '' THEN
		query = query || ' AND A.client_id like ''%'||client_id||'%''';
	end if;
	
	query = query || 'GROUP BY A.client_name,A.client_id,A.on_time,A.off_time,B.name ORDER BY A.on_time DESC limit '||page_size||' offset '||page_index;
	FOR rec in EXECUTE(query) loop
					rec.alarm_num = (SELECT COUNT(alarm.*) FROM icc_t_alarm alarm WHERE alarm.time BETWEEN rec.online_time and  CASE WHEN rec.offline_time IS NULL THEN now() ELSE rec.offline_time END AND alarm.receipt_seatno = rec.client_id );
					return next rec;
					end loop;
return;
end;
$$;

SET default_tablespace = '';
SET default_with_oids = false;

CREATE FUNCTION query_statistic_info(begin_time timestamp without time zone, end_time timestamp without time zone, page_size integer, page_index integer, user_dept character varying, client_name character varying, client_id character varying) RETURNS SETOF statistic_result
    LANGUAGE plpgsql
    AS $$
declare 
	rec statistic_result%rowtype;
	query text;
BEGIN
  query = 'SELECT *,count(*) over() FROM icc_t_statistic s where online_time between '''||begin_time||''' and '''||end_time||''' and s.dept_name = any (SELECT dept.code FROM icc_view_dept_recursive dept WHERE dept."path" like ''%'||user_dept||'%'')';
	
	if client_name != '' THEN
		query = query || ' AND s.client_name = '''|| client_name ||'''';
	end if;
	
	if client_id != '' THEN
		query = query || ' AND s.client_id like ''%'||client_id||'%''';
	end if;
	
	query = query || ' ORDER BY s.online_time DESC limit '||page_size||' offset '||page_index;
	
	FOR rec in EXECUTE(query) loop
			if rec.offline_time is null then
				 rec.online_time_len = ROUND(EXTRACT(EPOCH FROM (NOW()-rec.online_time)));				
				 rec.offseat_time_len = GREATEST(0,ROUND(EXTRACT(EPOCH FROM (SELECT NOW()-rec.online_time-SUM(least(B.logout_time,now()) - B.login_time) FROM icc_t_client_in_out B where B.login_time BETWEEN rec.online_time AND now() AND B.client_id=rec.client_id AND B.client_name = rec.client_name ))));
			end if;
			if 'DIC014002'=(select type from icc_t_dept where code=rec.dept_name) then
				rec.client_id='';
			end if;
			return next rec;			
	end loop;
return;
end;
$$;

CREATE FUNCTION statistic_record_online_info(param_guid character varying,param_client_name character varying,param_client_id character varying,param_dept_code character varying,param_on_time timestamp without time zone) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	insert into icc_t_statistic(guid,client_name,client_id,dept_name,online_time,online_time_len,offseat_time_len,release_call_num,total_call_num,alarm_num) 	
	values(param_guid,param_client_name,param_client_id,param_dept_code,param_on_time,0,0,0,0,0);	
END;
$$;


CREATE FUNCTION statistic_the_world() RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	query text;		
	rec icc_t_statistic%rowtype;
BEGIN
	query='SELECT	A.guid,A.client_name,A.client_id,B.code AS dept_name,
					A.on_time AS online_time,
					A.off_time AS offline_time,
					ROUND(EXTRACT ( EPOCH FROM CASE WHEN A.off_time IS NULL THEN now() ELSE A.off_time END - A.on_time )) AS online_time_len,
					GREATEST(0,ROUND(EXTRACT(EPOCH FROM (SELECT LEAST(A.off_time,NOW())-A.on_time-SUM(least(B.logout_time,LEAST(A.off_time,now())) - B.login_time) FROM icc_t_client_in_out B where 	B.login_time BETWEEN A.on_time AND LEAST(A.off_time,now()) AND B.client_id=A.client_id AND B.client_name = A.client_name )))) AS offseat_time_len,
					COUNT (DISTINCT CASE WHEN C.hangup_type = ''release'' THEN C.callref_id ELSE NULL END) AS release_call_num,
					COUNT(DISTINCT C.callref_id ) AS total_call_num,
					0 as alarm_num		
		FROM
					icc_t_client_on_off A LEFT JOIN icc_t_callevent C ON ((C.called_id = A.client_id AND C.call_direction = ''in'') OR (C.caller_id = A.client_id AND C.call_direction=''out'')) AND C.ring_time BETWEEN A.on_time AND CASE WHEN A.off_time IS NULL THEN now() ELSE A.off_time END
					LEFT JOIN icc_t_dept B ON B.code = A.dept_code WHERE A.guid not in (SELECT guid FROM icc_t_statistic)			
					GROUP BY A.guid,A.client_name,A.client_id,A.on_time,A.off_time,B.code';
	for rec in EXECUTE(query) LOOP
			rec.alarm_num = (SELECT count(alarm.id) from icc_t_alarm alarm where alarm.receipt_seatno = rec.client_id and alarm.time between rec.online_time and least(rec.offline_time,now()));
			insert into icc_t_statistic values(rec.*);
	END LOOP;
END;
$$;

CREATE FUNCTION statistic_update(param_guid character varying) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	row_update statistic_type_update;
	query text;
BEGIN
	
	SELECT A.client_name,A.client_id,
				A.on_time AS online_time,
				A.off_time AS offline_time,
				ROUND(EXTRACT ( EPOCH FROM CASE WHEN A.off_time IS NULL THEN now() ELSE A.off_time END - A.on_time )) AS online_time_length,
				ROUND(EXTRACT(EPOCH FROM (SELECT LEAST(A.off_time,NOW())-A.on_time-SUM(least(B.logout_time,LEAST(A.off_time,now())) - B.login_time) FROM icc_t_client_in_out B where B.login_time BETWEEN A.on_time AND CASE WHEN A.off_time is null THEN now() ELSE A.off_time END AND B.client_id=A.client_id AND B.client_name = A.client_name ))) AS offseat_time_length,
				COUNT (DISTINCT CASE WHEN C.hangup_type = 'release' THEN C.callref_id ELSE NULL END) AS release_callref_count,
				COUNT(DISTINCT C.callref_id ) AS callref_total_count,
				COUNT(DISTINCT alarm.*) AS alarm_count
	INTO row_update
	FROM
				icc_t_client_on_off A LEFT JOIN icc_t_callevent C ON ((C.called_id = A.client_id ) OR (C.caller_id = A.client_id )) AND C.hangup_time BETWEEN A.on_time AND CASE WHEN A.off_time IS NULL THEN now() ELSE A.off_time END	
				LEFT JOIN icc_t_alarm alarm ON alarm.receipt_seatno = A.client_id AND alarm.time BETWEEN A.on_time AND CASE WHEN A.off_time IS NULL THEN now() ELSE A.off_time END				
        WHERE A.guid = param_guid group by client_name,client_id,on_time,off_time limit 1 offset 0;	
	update icc_t_statistic set offline_time = row_update.offline_time,online_time_len= row_update.online_time_length,offseat_time_len = GREATEST(row_update.offseat_time_length,0),release_call_num = row_update.release_callref_count,total_call_num = row_update.callref_total_count,alarm_num = row_update.alarm_count  where guid = param_guid;
	return ;
END;
$$;

CREATE FUNCTION statistic_update_alarm_num(param_receipt_seatno character varying) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN	
	update icc_t_statistic s set alarm_num = alarm_num+1 where s.offline_time is null and (s.client_id = param_receipt_seatno);
	return ;
END;
$$;

CREATE FUNCTION statistic_update_call_num(param_hangup_type character varying,param_caller_id character varying,param_called_id character varying,param_call_direction character varying) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN	
	update icc_t_statistic s set release_call_num = case param_hangup_type when 'release' then release_call_num+1 else release_call_num end,
	total_call_num = case param_hangup_type when 'release' then total_call_num else total_call_num+1 end
	where s.offline_time is null and ((param_called_id = s.client_id ) OR (s.client_id = param_caller_id ));
	return ;
END;
$$;

CREATE OR REPLACE FUNCTION decode(VARIADIC p_decode_list _text)
	RETURNS pg_catalog.text
	LANGUAGE plpgsql
	AS $$
declare
	v_len integer := array_length(p_decode_list, 1);
	v_ret text;
begin
 if v_len >= 3 then
  if(p_decode_list[1] is NULL) then
	v_ret=p_decode_list[v_len];
  else
  for i in 2..(v_len - 1) loop
   v_ret := null;
   if mod(i, 2) = 0 then
    if p_decode_list[1] = p_decode_list[i] then
     v_ret := p_decode_list[i+1];
    elsif p_decode_list[1] <> p_decode_list[i] then
     if v_len = i + 2 and v_len > 3 then
      v_ret := p_decode_list[v_len];
     end if;
    end if;
   end if;
   exit when v_ret is not null;
  end loop;
	end if;
 else
  raise exception 'UPG-00938: not enough args for function.';
 end if;
 return v_ret;
end;
$$;
