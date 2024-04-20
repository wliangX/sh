-----------------------------------------------------------icc_t_jjdb�ֱ��ʼ��-----------------------------------------------------------
DROP FUNCTION IF exists shard_icc_t_jjdb();
CREATE OR REPLACE FUNCTION shard_icc_t_jjdb()
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
	min_time := (SELECT MIN(jjsj) FROM icc_t_jjdb_dt);
	if min_time is null then
		min_time := (select date_trunc('month',current_date)::DATE);
	else 
		min_time := (select date_trunc('month',min_time)::DATE);
	end if;
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||back_table_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'ALTER TABLE IF EXISTS '||final_name||' RENAME TO '||back_table_name||';';
	EXECUTE strSQL;
	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			XZQHDM character varying(12),
			JJDWDM character varying(12),
			JJDBH character varying(64) NOT NULL,
			GLZJJDBH character varying(30),
			JJLX character varying(64),
			JQLYFS character varying(64),
			LHLX character varying(64),
			JJLYH character varying(255), 
			JJYBH character varying(30),
			JJYXM character varying(50),
			BJSJ timestamp(6) without time zone,
			JJSJ timestamp(6) without time zone, 
			JJWCSJ timestamp(6) without time zone,
			BJDH character varying(50),
			BJDHYHM character varying(50),
			BJDHYHDZ text,
			BJRMC character varying(200),
			BJRXBDM character varying(64),
			LXDH character varying(50),
			BJRZJDM character varying(32),
			BJRZJHM character varying(96),
			BJDZ text,
			JQDZ text,
			BJNR character varying(4000),
			GXDWDM character varying(12),
			JQLBDM character varying(8),
			JQLXDM character varying(8),
			JQXLDM character varying(8),
			JQZLDM character varying(8),
			TZDBH character varying(50),
			ZARS numeric,
			YWCWQ character varying(64),
			YWWXWZ character varying(64),
			YWBZXL character varying(64),
			BKRYQKSM character varying(500),
			SSRYQKSM character varying(500),
			SWRYQKSM character varying(500),
			SFSWYBJ character varying(64),
			BJRXZB numeric,
			BJRYZB numeric,
			FXDWJD numeric,
			FXDWWD numeric,
			BCJJNR character varying(2000),
			JQDJDM character varying(64),
			JQCLZTDM character varying(64),
			YJJYDJDM character varying(64),
			SJCHPZLDM character varying(64),
			SJCPH character varying(100),
			SFSWHCL character varying(64),
			CJSJ timestamp(6) without time zone,
			GXSJ timestamp(6) without time zone,
			JQBQ character varying(800),
			SFBM character varying(64),
			GXDWMC character varying(200),
			GXDWDMBS character varying(200),
			DQYWZT character varying(200),
			JJDWMC character varying(200),
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
			GXZDBS character varying(50)
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
		strSQL := 'COMMENT ON COLUMN public.'||final_name||'.XZQHDM
				IS ''������������(�������ڵ������������룬��ʶ���ء���һ��������ؼ�����Ҳ���м�ָ�����ĽӾ������ɾ������������Ӧ�Ǳ��������е���������)'';

				COMMENT ON COLUMN public.'||final_name||'.JJDWDM
					IS ''�Ӿ���λ����(�Ӿ���λһ���ǵ��ع�������110��������̨��Ҳ���������ԽӾ���λ)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJDBH 
					IS ''�Ӿ������(�Ӿ�����ţ���������Ϊ����������������+ϵͳ�Ӿ�����ţ�
					�Ա��ⲻͬ��������ͬ����ţ�Ψһ�ţ����������������ڹ����ɾ�������������)'';
					
				COMMENT ON COLUMN public.'||final_name||'.GLZJJDBH 
					IS ''�������Ӿ������(��ǰ�Ӿ��������������Ӿ����ı�š�)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJLX
					IS ''�Ӿ�����(�Ӿ����ʹ��룬���־�������110��122���ԽӾ��������Ӿ���)'';
	
				COMMENT ON COLUMN public.'||final_name||'.JQLYFS
					IS ''������Դ��ʽ(������Դ��ʽ������˵���������ķ�ʽ�������绰���������ű��������籨���ȡ�)'';
	
				COMMENT ON COLUMN public.'||final_name||'.LHLX
					IS ''��������(�������ʹ��룬���������������Ļ������ࡣ)'';
	
				COMMENT ON COLUMN public.'||final_name||'.JJLYH
					IS ''�Ӿ�¼����(�Ӿ�¼���ţ��Ӿ�ʱ��¼��ϵͳ�Զ��������Ӵ���ϵͳ�Զ�������)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJYBH
					IS ''�Ӿ�Ա���(�Ӿ�Ա��ţ�������ʶ�Ӿ�Ա��ݡ�)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJYXM
					IS ''�Ӿ�Ա����'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJSJ
					IS ''����ʱ��(�绰����ʱ��Ϊ�Ŷӵ��Ȼ��������绰���䵽�Ӿ�̨��ʱ�䣻����������ʽʱ��Ϊʵ�ʱ���ʱ��)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJSJ
					IS ''�Ӿ�ʱ��(�绰����ʱΪ�Ӿ�Աժ�绰��Ӧ��ʱ�䣻���౨����ʽʱΪ¼����Կ�ʼʱ��)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJWCSJ
					IS ''�Ӿ����ʱ��(��������ʱΪ�Ӿ�Ա�Ҷϵ绰��ʱ�䣬����������ʽʱΪ����Ӿ����˳�ʱ��)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJDH
					IS ''�����绰(�����绰���룬���ֶ���Ϣ֮һ)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJDHYHM
					IS ''�����绰�û���(�����绰�û����������ֶ���Ϣ֮һ������ʱ���ɲ��ϵͳ�Զ�����)'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJDHYHDZ
					IS ''�����绰�û���ַ(�����绰�û���ַ�����ֶ���Ϣ֮һ������ʱ���ɲ��ϵͳ�Զ�����)'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJRMC
					IS ''����������(�������������������򼼷��������ơ��豸��ŵȣ�����������˱�������200λ��Ĭ��Ϊ��������)'';
						
				COMMENT ON COLUMN public.'||final_name||'.BJRXBDM
					IS ''�������Ա����'';
					
				COMMENT ON COLUMN public.'||final_name||'.LXDH
					IS ''��ϵ�绰(��������ϵ�绰��Ĭ��Ϊ�����绰)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJRZJDM
					IS ''������֤������(������֤����������)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJRZJHM
					IS ''������֤������(��������Ч֤������)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJDZ
					IS ''������ַ(���������ڵ�ַ����λ���ȡ�)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQDZ
					IS ''�����ַ(���鷢���صĵ�ַ����λ����)'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJNR 
					IS ''��������(�������ݵ���ϸ��������)'';
	
				COMMENT ON COLUMN public.'||final_name||'.GXDWDM
					IS ''��Ͻ��λ����(�����Ͻ����λ�Ļ�������)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQLBDM
					IS ''����������(���������룬���������������ʵ�һ�����ʹ���)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQLXDM
					IS ''�������ʹ���(�������ʹ��룬���������������ʵĶ������ʹ���)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQXLDM
					IS ''����ϸ�����(����ϸ����룬���������������ʵ��������ʹ���)'';
	
				COMMENT ON COLUMN public.'||final_name||'.JQZLDM
					IS ''�����������(����������룬���������������ʵ��ļ����ʹ���)'';
	
				COMMENT ON COLUMN public.'||final_name||'.TZDBH
					IS ''��������(�������ţ���·�Ƹˡ���Ƶ��װ��ȸ�����λ��ʩ�ı��)'';
					
				COMMENT ON COLUMN public.'||final_name||'.ZARS
					IS ''��������(Υ����������������)'';
					
				COMMENT ON COLUMN public.'||final_name||'.YWCWQ
					IS ''���޳�����(0���ޣ�1���У�Ĭ��Ϊ0)'';
					
				COMMENT ON COLUMN public.'||final_name||'.YWWXWZ
					IS ''����Σ������(0���ޣ�1���У�Ĭ��Ϊ0)'';
					
				COMMENT ON COLUMN public.'||final_name||'.YWBZXL
					IS ''���ޱ�ը/й©(0���ޣ�1���У�Ĭ��Ϊ0)'';
					
				COMMENT ON COLUMN public.'||final_name||'.BKRYQKSM
					IS ''������Ա���˵��'';
	
				COMMENT ON COLUMN public.'||final_name||'.SSRYQKSM
					IS ''������Ա���˵��'';
					
				COMMENT ON COLUMN public.'||final_name||'.SWRYQKSM
					IS ''������Ա���˵��'';
					
				COMMENT ON COLUMN public.'||final_name||'.SFSWYBJ
					IS ''�Ƿ������ﱨ��'';
	
				COMMENT ON COLUMN public.'||final_name||'.BJRXZB
					IS ''�����˶�λX����'';
					
				COMMENT ON COLUMN public.'||final_name||'.BJRYZB
					IS ''�����˶�λY����'';
	
				COMMENT ON COLUMN public.'||final_name||'.FXDWJD
					IS ''����λX����'';
					
				COMMENT ON COLUMN public.'||final_name||'.FXDWWD
					IS ''����λY����'';
					
				COMMENT ON COLUMN public.'||final_name||'.BCJJNR
					IS ''����Ӿ�����'';	
					
				COMMENT ON COLUMN public.'||final_name||'.JQDJDM
					IS ''����ȼ�����'';	
				 
				COMMENT ON COLUMN public.'||final_name||'.JQCLZTDM
					IS ''���鴦��״̬����(��¼��������Ŀǰ��ҵ��״̬)'';	
					
				COMMENT ON COLUMN public.'||final_name||'.YJJYDJDM
					IS ''Ӧ����Ԯ�ȼ�����'';
	
				COMMENT ON COLUMN public.'||final_name||'.SJCHPZLDM
					IS ''�漰�������������'';
					
				COMMENT ON COLUMN public.'||final_name||'.SJCPH
					IS ''�漰���ƺ�'';
					
				COMMENT ON COLUMN public.'||final_name||'.SFSWHCL
					IS ''�Ƿ���Σ������(0����1���ǣ�Ĭ��Ϊ0)'';
	
				COMMENT ON COLUMN public.'||final_name||'.CJSJ
					IS ''����ʱ��(��¼������ʱ��)'';
	
				COMMENT ON COLUMN public.'||final_name||'.GXSJ
					IS ''����ʱ��(��¼�����������ʱ��)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JQBQ
					IS ''�����ǩ(�����ǩ�ǶԾ������ݹؼ�Ҫ�ص���ȡ�ͱ�ע��)'';
	
				COMMENT ON COLUMN public.'||final_name||'.SFBM
					IS ''�Ƿ���(0����1���ǣ�Ĭ��Ϊ0)'';  
					
				COMMENT ON COLUMN public.'||final_name||'.JJFS
					IS ''�Ӿ�����(������vcs�Խӡ�icc�Ӿ���mpa�Ӿ�)'';
					
				COMMENT ON COLUMN public.'||final_name||'.GXDWMC
					IS ''��Ͻ��λ����'';	
	
				COMMENT ON COLUMN public.'||final_name||'.GXDWDMBS
					IS ''��Ͻ��λ����'';	
	
				COMMENT ON COLUMN public.'||final_name||'.JJDWMC
					IS ''�Ӿ���λ����'';	
	
				COMMENT ON COLUMN public.'||final_name||'.JJDWDMBS
					IS ''�Ӿ���λ����'';	
					
				COMMENT ON COLUMN public.'||final_name||'.SCBS
					IS ''ɾ�����λ'';	
	
				COMMENT ON COLUMN public.'||final_name||'.WXBS
					IS ''��Ч�������λ'';	
					
				COMMENT ON COLUMN public.'||final_name||'.DQYWZT
					IS ''��ǰҵ��״̬(vcsҵ��״̬)'';
					
				COMMENT ON COLUMN public.'||final_name||'.JJXWH
					IS ''����Ӿ���ϯλ��'';
					
				COMMENT ON COLUMN public.'||final_name||'.HBBS
					IS ''�Ƿ�ϲ�(�Ƿ�ϲ�1���ϲ���0��δ�ϲ�)'';	
					
				COMMENT ON COLUMN public.'||final_name||'.CJRY
					IS ''������'';	
					
				COMMENT ON COLUMN public.'||final_name||'.GXRY
					IS ''������(�޸���,ȡ���һ���޸�ֵ�޸�ʱ��,ȡ���һ���޸�ֵ)'';
	
				COMMENT ON COLUMN public.'||final_name||'.HFBS
					IS ''�Ƿ�ط�'';
					
				COMMENT ON COLUMN public.'||final_name||'.FKBS
					IS ''�Ƿ���'';
	
				COMMENT ON COLUMN public.'||final_name||'.gllx 
					IS ''�����������ͣ��ջ�0û������1�ظ�������2������߰죻3���񾯷�����4�����ɧ�ţ���'';
	
				COMMENT ON COLUMN public.'||final_name||'.CJZDBS
					IS ''�����ն˱�ʾ'';
	
				COMMENT ON COLUMN public.'||final_name||'.GXZDBS
					IS ''�����ն˱�ʾ'';	
					
				COMMENT ON COLUMN public.'||final_name||'.tzhgxdwdmbs 
					IS ''������Ĺ�Ͻ��λ����'';
					
				COMMENT ON COLUMN public.'||final_name||'.ybjxxyz
					IS ''�Ƿ��뱳����Ϣһ�� 0����һ�£�1��һ�� ybjxxyz'';
					
				COMMENT ON COLUMN public.'||final_name||'.tbcjbs
					IS ''ͬ��������ʶ'';
					
				COMMENT ON COLUMN public.'||final_name||'.tzhgxdwdm
					IS ''������Ĺ�Ͻ��λ����'';
					
				COMMENT ON COLUMN public.'||final_name||'.tzhgxdwmc 
					IS ''������Ĺ�Ͻ��λ����'';
					
				COMMENT ON COLUMN public.'||final_name||'.sfja 
					IS ''�Ƿ�᰸'';';
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
				  create index '||index_name||'_'||min_time_begin_short_str||'_crttime on '||final_name||'_'||min_time_begin_short_str||' (cjsj);';
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	strSQL := 'insert into '||final_name||' select * from icc_t_jjdb_dt where '||submeter_key||' is not null;';
	EXECUTE strSQL;
	
	DROP TABLE IF EXISTS "public"."icc_t_jjdb_dt";
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

DROP TABLE IF EXISTS "public"."icc_t_jjdb_dt" CASCADE;
CREATE TABLE icc_t_jjdb_dt as (SELECT * FROM icc_t_jjdb);
select shard_icc_t_jjdb();

-----------------------------------------------------------icc_t_pjdb�ֱ��ʼ��-----------------------------------------------------------
DROP FUNCTION IF exists shard_icc_t_pjdb();
CREATE OR REPLACE FUNCTION shard_icc_t_pjdb()
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
	for dt_data in execute 'select jjsj, jjdbh from icc_t_jjdb' loop 
		dt_jjsj := dt_data."jjsj";
		dt_data_str := dt_data."jjdbh";
		strSQL := 'update icc_t_pjdb_dt set "jjsj" = '''||dt_jjsj||''' where "jjdbh" = '''||dt_data_str||''';';
	    execute strSQL;
	end loop;

	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (SELECT MIN(jjsj) FROM icc_t_pjdb_dt);
	if min_time is null then
		min_time := (select date_trunc('month',current_date)::DATE);
	else 
		min_time := (select date_trunc('month',min_time)::DATE);
	end if;
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||back_table_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'ALTER TABLE IF EXISTS '||final_name||' RENAME TO '||back_table_name||';';
	EXECUTE strSQL;
	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			xzqhdm character varying(12),
			pjdbh character varying(64) NOT NULL,
			jjdbh character varying(64),
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
			qsrym character varying(30),
			cdry character varying(200), 
			cdcl character varying(500),
			cdct character varying(500),
			jqclztdm character varying(64),
			cjsj timestamp(6) without time zone,
			gxsj timestamp(6) without time zone,
			dqywzt character varying(32),
			cszt character varying(32),
			pjdwmc character varying(128),
			pjdwdmbs character varying(200),
			cjdxid character varying(64),
			cjdxdm character varying(200),
			cjdxmc character varying(128),
			cjdxlx character varying(32),
			cjdwmc character varying(128),
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
			jjsj timestamp(6) without time zone
			)partition by range('||submeter_key||');';
		EXECUTE strSQL;
	
		strSQL :=  'COMMENT ON COLUMN public.'||final_name||'.xzqhdm IS ''�ɾ���λ��������'';
					COMMENT ON COLUMN public.'||final_name||'.pjdbh IS ''�ɾ������'';
					COMMENT ON COLUMN public.'||final_name||'.jjdbh IS ''�Ӿ������'';
					COMMENT ON COLUMN public.'||final_name||'.pjdwdm IS ''�´��ɾ�ָ��ĵ�λ��������'';
					COMMENT ON COLUMN public.'||final_name||'.pjybh IS ''�ɾ�Ա��ţ�������ʶ�ɾ�Ա���'';
					COMMENT ON COLUMN public.'||final_name||'.pjyxm IS ''�ɾ�Ա����'';
					COMMENT ON COLUMN public.'||final_name||'.pjlyh IS ''�ɾ�¼���ţ��ɾ�ʱ��¼��ϵͳ�Զ��������Ӵ���ϵͳ�Զ�����'';
					COMMENT ON COLUMN public.'||final_name||'.pjyj IS ''�ɾ��������ϸ��������'';
					COMMENT ON COLUMN public.'||final_name||'.cjdwdm IS ''������λ��������'';
					COMMENT ON COLUMN public.'||final_name||'.pjsj IS ''�ɾ�Ա�´��ɾ�����ʱ��'';
					COMMENT ON COLUMN public.'||final_name||'.jjsj IS ''�Ӿ������нӾ�ʱ��'';
					COMMENT ON COLUMN public.'||final_name||'.xtzdddsj IS ''������λ��Ӧ��ϵͳ�Զ����յ���ʱ��'';
					COMMENT ON COLUMN public.'||final_name||'.pdqssj IS ''�����´��ɾ����󣬳�����λ�ֹ�ȷ�Ͻӵ����ɾ�����ʱ��'';
					COMMENT ON COLUMN public.'||final_name||'.qsryxm IS ''ǩ�վ�����Ա����'';
					COMMENT ON COLUMN public.'||final_name||'.qsrym IS ''ǩ�վ�����Ա֤�����루���֤)'';
					COMMENT ON COLUMN public.'||final_name||'.cdry IS ''��¼������Ա��������ϸ��Ϣ'';
					COMMENT ON COLUMN public.'||final_name||'.cdcl IS ''��¼������������ϸ��Ϣ'';
					COMMENT ON COLUMN public.'||final_name||'.cdct IS ''��¼������ͧ����ϸ��Ϣ'';
					COMMENT ON COLUMN public.'||final_name||'.jqclztdm IS ''��¼��������Ŀǰ��ҵ��״̬'';
					COMMENT ON COLUMN public.'||final_name||'.cjsj IS ''��¼������ʱ��'';
					COMMENT ON COLUMN public.'||final_name||'.gxsj IS ''����ʱ��'';
					COMMENT ON COLUMN public.'||final_name||'.pjdwmc IS ''�ɾ���λ����'';
					COMMENT ON COLUMN public.'||final_name||'.pjdwdmbs IS ''�ɾ���λ����'';
					COMMENT ON COLUMN public.'||final_name||'.cjdwmc IS ''������λ����'';
					COMMENT ON COLUMN public.'||final_name||'.cjdwdmbs IS ''������λ����'';
					COMMENT ON COLUMN public.'||final_name||'.cjdxlx IS ''�����������ͣ���λ����Ա��������'';
					COMMENT ON COLUMN public.'||final_name||'.cjdxmc IS ''������������'';
					COMMENT ON COLUMN public.'||final_name||'.cjdxid IS ''�����������'';
					COMMENT ON COLUMN public.'||final_name||'.dqywzt IS ''vcs�ڲ�ҵ��״̬'';
					COMMENT ON COLUMN public.'||final_name||'.qsrydm IS ''ǩ����Ա����(��ϯ���)'';
					COMMENT ON COLUMN public.'||final_name||'.tdsj IS ''�˵�����ʱ��'';
					COMMENT ON COLUMN public.'||final_name||'.cxyy IS ''ȡ��ԭ��'';
					COMMENT ON COLUMN public.'||final_name||'.sfzddpcj IS ''�Ƿ��Զ��������񴴽�'';
					COMMENT ON COLUMN public.'||final_name||'.cjry IS ''������'';
					COMMENT ON COLUMN public.'||final_name||'.gxry IS ''������'';
					COMMENT ON COLUMN public.'||final_name||'.cszt IS ''��ʱ״̬'';
					COMMENT ON COLUMN public.'||final_name||'.CJZDBS IS ''�����ն˱�ʾ'';
					COMMENT ON COLUMN public.'||final_name||'.GXZDBS IS ''�����ն˱�ʾ'';';
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
				  create index '||index_name||'_'||min_time_begin_short_str||'_crttime on '||final_name||'_'||min_time_begin_short_str||'(cjsj);
				  create index '||index_name||'_'||min_time_begin_short_str||'_uptime on '||final_name||'_'||min_time_begin_short_str||'(gxsj);';		 
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	strSQL := 'insert into '||final_name||' select * from icc_t_pjdb_dt where '||submeter_key||' is not null;';
	EXECUTE strSQL;
	
	DROP TABLE IF EXISTS "public"."icc_t_pjdb_dt";
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

DROP TABLE IF EXISTS "public"."icc_t_pjdb_dt" CASCADE;
CREATE TABLE icc_t_pjdb_dt as (SELECT * FROM icc_t_pjdb);
ALTER TABLE public.icc_t_pjdb_dt ADD jjsj timestamp(6);
select shard_icc_t_pjdb();

-----------------------------------------------------------icc_t_alarm_log�ֱ��ʼ��-----------------------------------------------------------
DROP FUNCTION IF exists shard_icc_t_alarm_log();
CREATE OR REPLACE FUNCTION shard_icc_t_alarm_log()
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
	for dt_data in execute 'select jjsj, jjdbh from icc_t_jjdb' loop 
		dt_jjsj := dt_data."jjsj";
		dt_data_str := dt_data."jjdbh";
		strSQL := 'update icc_t_alarm_log_dt set "jjsj" = '''||dt_jjsj||''' where "jjdbh" = '''||dt_data_str||''';';
	    execute strSQL;
	end loop;

	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (SELECT MIN(jjsj) FROM icc_t_alarm_log_dt);
	if min_time is null then
		min_time := (select date_trunc('month',current_date)::DATE);
	else 
		min_time := (select date_trunc('month',min_time)::DATE);
	end if;
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||back_table_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'ALTER TABLE IF EXISTS '||final_name||' RENAME TO '||back_table_name||';';
	EXECUTE strSQL;
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
	
	strSQL := 'insert into '||final_name||' select * from icc_t_alarm_log_dt where '||submeter_key||' is not null;';
	EXECUTE strSQL;
	
	DROP TABLE IF EXISTS "public"."icc_t_alarm_log_dt";
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

DROP TABLE IF EXISTS "public"."icc_t_alarm_log_dt" CASCADE;
CREATE TABLE icc_t_alarm_log_dt as (SELECT * FROM icc_t_alarm_log);
ALTER TABLE public.icc_t_alarm_log_dt ADD jjsj timestamp(6);
select shard_icc_t_alarm_log();

-----------------------------------------------------------icc_t_callevent�ֱ��ʼ��--------------------------------------------------------------
DROP FUNCTION IF exists isnumeric();
CREATE OR REPLACE FUNCTION isnumeric (txtStr VARCHAR) 
RETURNS BOOLEAN AS $$
BEGIN
RETURN txtStr ~ '^([0-9]+[.]?[0-9]*|[.][0-9]+)$' ;
END; 
$$ LANGUAGE 'plpgsql';

DROP FUNCTION IF exists shard_icc_t_callevent();
CREATE OR REPLACE FUNCTION shard_icc_t_callevent()
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
	ALTER TABLE public.icc_t_callevent_dt ADD create_time timestamp(6);
	ALTER TABLE public.icc_t_callevent_dt ADD incoming_len interval;
	ALTER TABLE public.icc_t_callevent_dt ADD ring_len interval;
	ALTER TABLE public.icc_t_callevent_dt ADD talk_len interval;
	ALTER TABLE public.icc_t_callevent ADD record_file_id text;
	COMMENT ON COLUMN "public"."icc_t_callevent"."record_file_id" IS '¼���ļ�ID';
	for dt_data in execute 'select callref_id,incoming_time,ring_time,talk_time,hangup_time,ringback_time from icc_t_callevent_dt where isnumeric(callref_id)=true' loop 
		dt_data_str := dt_data."callref_id";
		dt_year := SUBSTRING(dt_data_str, 1, 4);
		dt_month := SUBSTRING(dt_data_str, 5, 2);
		dt_day := SUBSTRING(dt_data_str, 7, 2);
		dt_hour := SUBSTRING(dt_data_str, 9, 2);
		dt_minute := SUBSTRING(dt_data_str, 11, 2);
		dt_sec := SUBSTRING(dt_data_str, 13, 2);
		dt_str_jjsj := ''||dt_year||'-'||dt_month||'-'||dt_day||' '||dt_hour||':'||dt_minute||':'||dt_sec||'';
		dt_jjsj := to_timestamp(dt_str_jjsj, 'yyyy-mm-dd hh24:mi:ss');
		dt_create_time_start := dt_jjsj - interval'10 sec';
		dt_create_time_end := dt_jjsj + interval'10 sec';
		if dt_data."incoming_time" is not null and dt_data."ring_time" is not null then
			dt_incomming_len := dt_data."ring_time" - dt_data."incoming_time";
			strSQL := 'update icc_t_callevent_dt set "incoming_len" = '''||dt_incomming_len||''' where "callref_id" = '''||dt_data_str||''' and "create_time" >= '''||dt_create_time_start||''' and "create_time" <= '''||dt_create_time_end||''';';
		    execute strSQL;
		end if;
		if dt_data."talk_time" is not null and dt_data."ring_time" is not null then
			dt_ring_len := dt_data."talk_time" - dt_data."ring_time";
			strSQL := 'update icc_t_callevent_dt set "ring_len" = '''||dt_ring_len||''' where "callref_id" = '''||dt_data_str||''' and "create_time" >= '''||dt_create_time_start||''' and "create_time" <= '''||dt_create_time_end||''';';
	   	 	execute strSQL;
		elseif dt_data."talk_time" is not null and dt_data."ringback_time" is not null then
			dt_ring_len = dt_data."talk_time" - dt_data."ringback_time";
			strSQL := 'update icc_t_callevent_dt set "ring_len" = '''||dt_ring_len||''' where "callref_id" = '''||dt_data_str||''' and "create_time" >= '''||dt_create_time_start||''' and "create_time" <= '''||dt_create_time_end||''';';
	    	execute strSQL;
		end if;
	
		if dt_data."hangup_time" is not null and dt_data."talk_time" is not null then
			dt_talk_len := dt_data."hangup_time" - dt_data."talk_time";
			strSQL := 'update icc_t_callevent_dt set "talk_len" = '''||dt_talk_len||''' where "callref_id" = '''||dt_data_str||''' and "create_time" >= '''||dt_create_time_start||''' and "create_time" <= '''||dt_create_time_end||''';';
	    	execute strSQL;
		end if;
	
		strSQL := 'update icc_t_callevent_dt set "create_time" = '''||dt_jjsj||''' where "callref_id" = '''||dt_data_str||''';';
	    execute strSQL;
	end loop;
	
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (SELECT MIN(create_time) FROM icc_t_callevent_dt);
	if min_time is null then
		min_time := (select date_trunc('month',current_date)::DATE);
	else 
		min_time := (select date_trunc('month',min_time)::DATE);
	end if;
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
			talk_len interval
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
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_hangup_type ON '||final_name||'_'||min_time_begin_short_str||' USING btree (hangup_type);';		 
		EXECUTE strSQL;
		min_time_begin := min_time;
		min_time := min_time + interval '1 month';
		time_difference := cur_time - min_time + '1 month';
	END LOOP;
	
	strSQL := 'insert into '||final_name||' select * from icc_t_callevent_dt where '||submeter_key||' is not null;';
	EXECUTE strSQL;
	
	DROP TABLE IF EXISTS "public"."icc_t_callevent_dt";
	
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

DROP TABLE IF EXISTS "public"."icc_t_callevent_dt";
CREATE TABLE icc_t_callevent_dt as (SELECT * FROM icc_t_callevent);
select shard_icc_t_callevent();

-----------------------------------------------------------icc_t_fkdb�ֱ��ʼ��-----------------------------------------------------------
DROP FUNCTION IF exists shard_icc_t_fkdb();
CREATE OR REPLACE FUNCTION shard_icc_t_fkdb()
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
	ALTER TABLE public.icc_t_fkdb_dt ADD jjsj timestamp(6);
	min_time := (SELECT MIN(cjsj) FROM icc_t_fkdb);
	min_time := min_time - interval'1 month';
	for dt_data in execute 'select b.jjsj, a.jjdbh from icc_t_fkdb a left join icc_t_jjdb b on a.jjdbh = b.jjdbh and a.cjsj >= '''||min_time||''' and b.jjsj >= '''||min_time||'''' loop 
		if dt_data."jjsj" is not null then
			dt_jjsj := dt_data."jjsj";
			dt_data_str := dt_data."jjdbh";
			strSQL := 'update icc_t_fkdb_dt set "jjsj" = '''||dt_jjsj||''' where "jjdbh" = '''||dt_data_str||''';';
		    execute strSQL;
	   	end if;
	end loop;

	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (SELECT MIN(jjsj) FROM icc_t_fkdb_dt);
	if min_time is null then
		min_time := (select date_trunc('month',current_date)::DATE);
	else 
		min_time := (select date_trunc('month',min_time)::DATE);
	end if;
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;

	strSQL := 'DROP TABLE IF EXISTS "public"."'||back_table_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'ALTER TABLE IF EXISTS '||final_name||' RENAME TO '||back_table_name||';';
	EXECUTE strSQL;
	strSQL := 'DROP TABLE IF EXISTS "public"."'||final_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'CREATE TABLE '||final_name||'(
			xzqhdm character varying(12),
			fkdbh character varying(64),
			jjdbh character varying(64),
			pjdbh character varying(64),
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
			jqclztdm character varying(64),
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
			gcfkbs boolean,
			jjsj timestamp(6) without time zone
			)partition by range('||submeter_key||');';
	EXECUTE strSQL;
	
	COMMENT ON COLUMN public.icc_t_fkdb.xzqhdm
    IS '������������';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdbh
	    IS '���������';
	COMMENT ON COLUMN public.icc_t_fkdb.jjdbh
	    IS '�Ӿ������';
	COMMENT ON COLUMN public.icc_t_fkdb.pjdbh
	    IS '�ɾ������';
	COMMENT ON COLUMN public.icc_t_fkdb.fklyh
	    IS '����¼����';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwdm
	    IS '������λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.fkybh
	    IS '����Ա���';
	COMMENT ON COLUMN public.icc_t_fkdb.fkyxm
	    IS '����Ա����';
	COMMENT ON COLUMN public.icc_t_fkdb.fksj
	    IS '����ʱ��';
	COMMENT ON COLUMN public.icc_t_fkdb.cjsj01
	    IS 'ʵ�ʳ���ʱ��';
	COMMENT ON COLUMN public.icc_t_fkdb.ddxcsj
	    IS '�����ֳ�ʱ��';
	COMMENT ON COLUMN public.icc_t_fkdb.xcclwbsj
	    IS '�ֳ��������ʱ��';
	COMMENT ON COLUMN public.icc_t_fkdb.jqlbdm
	    IS '����������';
	COMMENT ON COLUMN public.icc_t_fkdb.jqlxdm
	    IS '�������ʹ���';
	COMMENT ON COLUMN public.icc_t_fkdb.jqxldm
	    IS '����ϸ�����';
	COMMENT ON COLUMN public.icc_t_fkdb.jqzldm
	    IS '�����������';
	COMMENT ON COLUMN public.icc_t_fkdb.jqfssj
	    IS '���鷢��ʱ��';
	COMMENT ON COLUMN public.icc_t_fkdb.bjdz
	    IS '������ַ';
	COMMENT ON COLUMN public.icc_t_fkdb.jqdz
	    IS '�����ַ';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwxzb
	    IS '������λX����(�����ֳ�λ��)';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwyzb
	    IS '������λY����(�����ֳ�λ��)';
	COMMENT ON COLUMN public.icc_t_fkdb.cjczqk
	    IS '�����������';
	COMMENT ON COLUMN public.icc_t_fkdb.cdcc
	    IS '��������';
	COMMENT ON COLUMN public.icc_t_fkdb.cdrc
	    IS '�����˴�';
	COMMENT ON COLUMN public.icc_t_fkdb.cdct
	    IS '������ͧ';
	COMMENT ON COLUMN public.icc_t_fkdb.jzrs
	    IS '��������';
	COMMENT ON COLUMN public.icc_t_fkdb.jzrssm
	    IS '��������˵��';
	COMMENT ON COLUMN public.icc_t_fkdb.ssrs
	    IS '��������';
	COMMENT ON COLUMN public.icc_t_fkdb.ssrssm
	    IS '��������˵��';
	COMMENT ON COLUMN public.icc_t_fkdb.swrs
	    IS '��������';
	COMMENT ON COLUMN public.icc_t_fkdb.swrssm
	    IS '��������˵��';
	COMMENT ON COLUMN public.icc_t_fkdb.jqcljgdm
	    IS '���鴦��������';
	COMMENT ON COLUMN public.icc_t_fkdb.jqcljgsm
	    IS '���鴦����˵��';
	COMMENT ON COLUMN public.icc_t_fkdb.tqqkdm
	    IS '�����������';
	COMMENT ON COLUMN public.icc_t_fkdb.ssqkms
	    IS '��ʧ�������';
	COMMENT ON COLUMN public.icc_t_fkdb.zhrs
	    IS 'ץ������';
	COMMENT ON COLUMN public.icc_t_fkdb.sars
	    IS '�永����';
	COMMENT ON COLUMN public.icc_t_fkdb.tprs
	    IS '��������';
	COMMENT ON COLUMN public.icc_t_fkdb.jtsgxtdm
	    IS '��ͨ�¹���̬����';
	COMMENT ON COLUMN public.icc_t_fkdb.sfzzwxp
	    IS '�Ƿ�װ��Σ��Ʒ';
	COMMENT ON COLUMN public.icc_t_fkdb.jtsgccyydm
	    IS '��ͨ�¹ʳ���ԭ�����';
	COMMENT ON COLUMN public.icc_t_fkdb.njddm
	    IS '�ܼ��ȴ���';
	COMMENT ON COLUMN public.icc_t_fkdb.lmzkdm
	    IS '·��״������';
	COMMENT ON COLUMN public.icc_t_fkdb.shjdcs
	    IS '�𻵻�������';
	COMMENT ON COLUMN public.icc_t_fkdb.shfjdcs
	    IS '�𻵷ǻ�������';
	COMMENT ON COLUMN public.icc_t_fkdb.dllxdm
	    IS '��·���ʹ���';
	COMMENT ON COLUMN public.icc_t_fkdb.jqclztdm
	    IS '���鴦��״̬����';
	COMMENT ON COLUMN public.icc_t_fkdb.cjsj
	    IS '����ʱ��';
	COMMENT ON COLUMN public.icc_t_fkdb.gxsj
	    IS '����ʱ��';
	COMMENT ON COLUMN public.icc_t_fkdb.jdxz
	    IS '����(�ֵ�)';
	COMMENT ON COLUMN public.icc_t_fkdb.sdcs
	    IS '���ش���';
	COMMENT ON COLUMN public.icc_t_fkdb.sfqz
	    IS '�Ƿ�ȡ֤';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwdm
	    IS '�ƽ���λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwlxr
	    IS '�ƽ���λ��ϵ������';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwfkqk
	    IS '�ƽ���λ�������';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwfkqksm
	    IS '�ƽ���λ�������˵��';
	COMMENT ON COLUMN public.icc_t_fkdb.ajbh
	    IS '�������';
	COMMENT ON COLUMN public.icc_t_fkdb.nrbq
	    IS '�����ǩ';
	COMMENT ON COLUMN public.icc_t_fkdb.jqbwdm
	    IS '���鲿λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.jqfsdy
	    IS '���鷢������';
	COMMENT ON COLUMN public.icc_t_fkdb.forcedm
	    IS '��������';
	COMMENT ON COLUMN public.icc_t_fkdb.forcetype
	    IS '��������';
	COMMENT ON COLUMN public.icc_t_fkdb.dqywzt
	    IS '��ǰҵ��״̬';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwdmbs
	    IS '������λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.fkdwmc
	    IS '������λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.livestatus
	    IS '�ֳ�״̬';
	COMMENT ON COLUMN public.icc_t_fkdb.tjdwdmbs
	    IS '�ύ��λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.wxbs
	    IS '�Ƿ���Ч�����ʶ';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwmc
	    IS '�ƽ���λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwdmbs
	    IS '�ƽ���λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.tzhgxdwdm
	    IS '�������Ͻ��λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.tzhgxdwmc
	    IS '�������Ͻ��λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.tzhgxdwdmbs
	    IS '�������Ͻ��λ����';
	COMMENT ON COLUMN public.icc_t_fkdb.yjdwlxrdm
	    IS '�ƽ���λ��ϵ�˴���';
	COMMENT ON COLUMN public.icc_t_fkdb.gcfkbs
	    IS '���̷�����ʶ';

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
	
	strSQL := 'insert into '||final_name||' select * from icc_t_fkdb_dt where '||submeter_key||' is not null;';
	EXECUTE strSQL;
	
	DROP TABLE IF EXISTS "public"."icc_t_fkdb_dt";
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

DROP TABLE IF EXISTS "public"."icc_t_fkdb_dt" CASCADE;
CREATE TABLE icc_t_fkdb_dt as (SELECT * FROM icc_t_fkdb);
select shard_icc_t_fkdb();

-----------------------------------------------------------icc_t_sub_callevent�ֱ��ʼ��--------------------------------------------------------------
DROP FUNCTION IF exists isnumeric();
CREATE OR REPLACE FUNCTION isnumeric (txtStr VARCHAR) 
RETURNS BOOLEAN AS $$
BEGIN
RETURN txtStr ~ '^([0-9]+[.]?[0-9]*|[.][0-9]+)$' ;
END; 
$$ LANGUAGE 'plpgsql';

DROP FUNCTION IF exists shard_icc_t_sub_callevent();
CREATE OR REPLACE FUNCTION shard_icc_t_sub_callevent()
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
	for dt_data in execute 'select callref_id from icc_t_sub_callevent_dt where isnumeric(callref_id)=true' loop 
		dt_data_str := dt_data."callref_id";
		dt_year := SUBSTRING(dt_data_str, 1, 4);
		dt_month := SUBSTRING(dt_data_str, 5, 2);
		dt_day := SUBSTRING(dt_data_str, 7, 2);
		dt_hour := SUBSTRING(dt_data_str, 9, 2);
		dt_minute := SUBSTRING(dt_data_str, 11, 2);
		dt_sec := SUBSTRING(dt_data_str, 13, 2);
		dt_str_jjsj := ''||dt_year||'-'||dt_month||'-'||dt_day||' '||dt_hour||':'||dt_minute||':'||dt_sec||'';
		dt_jjsj := to_timestamp(dt_str_jjsj, 'yyyy-mm-dd hh24:mi:ss');
		strSQL := 'update icc_t_sub_callevent_dt set "create_time" = '''||dt_jjsj||''' where "callref_id" = '''||dt_data_str||''';';
	    execute strSQL;
	end loop;
	
	cur_time := (select date_trunc('month',current_date)::DATE);
    cur_time := cur_time + '1 month';
	min_time := (SELECT MIN(create_time) FROM icc_t_sub_callevent_dt);
	if min_time is null then
		min_time := (select date_trunc('month',current_date)::DATE);
	else 
		min_time := (select date_trunc('month',min_time)::DATE);
	end if;
	min_time_begin := min_time;
	min_time := min_time + '1 month';
	time_difference := cur_time - min_time_begin;
	
	strSQL := 'DROP TABLE IF EXISTS "public"."'||back_table_name||'" CASCADE;';
	EXECUTE strSQL;
	strSQL := 'ALTER TABLE IF EXISTS '||final_name||' RENAME TO '||back_table_name||';';
	EXECUTE strSQL;
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
	
	strSQL := 'insert into '||final_name||' select * from icc_t_sub_callevent_dt where '||submeter_key||' is not null;';
	EXECUTE strSQL;
	
	DROP TABLE IF EXISTS "public"."icc_t_sub_callevent_dt";
	
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

DROP TABLE IF EXISTS "public"."icc_t_sub_callevent_dt" CASCADE;
CREATE TABLE icc_t_sub_callevent_dt as (SELECT * FROM icc_t_sub_callevent);
ALTER TABLE public.icc_t_sub_callevent_dt ADD create_time timestamp(6);
select shard_icc_t_sub_callevent();


-----------------------------------------------------------icc_t_jjdb�������·�����--------------------------------------------------------------
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
				  create index '||index_name||'_'||min_time_begin_short_str||'_crttime on '||final_name||'_'||min_time_begin_short_str||' (cjsj);';
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;


-----------------------------------------------------------icc_t_pjdb�������·�����--------------------------------------------------------------
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
				  create index '||index_name||'_'||min_time_begin_short_str||'_crttime on '||final_name||'_'||min_time_begin_short_str||'(cjsj);
				  create index '||index_name||'_'||min_time_begin_short_str||'_uptime on '||final_name||'_'||min_time_begin_short_str||'(gxsj);';		 
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;
-----------------------------------------------------------icc_t_alarm_log�������·�����--------------------------------------------------------------
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
-----------------------------------------------------------icc_t_callevent�������·�����--------------------------------------------------------------
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
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_hangup_type ON '||final_name||'_'||min_time_begin_short_str||' USING btree (hangup_type);';		 
		EXECUTE strSQL;
		final_result := '0';
	else 
		final_result := '1';
	end if;
	return final_result;
end;
$sql_str$ LANGUAGE plpgsql;

-----------------------------------------------------------icc_t_sub_callevent�������·�����--------------------------------------------------------------
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

select create_shardtable_icc_t_jjdb() as icc_t_jjdb, create_shardtable_icc_t_pjdb() as icc_t_pjdb, create_shardtable_icc_t_alarm_log() as icc_t_alarm_log, create_shardtable_icc_t_sub_callevent() as icc_t_sub_callevent, create_shardtable_icc_t_callevent() as icc_t_callevent;

