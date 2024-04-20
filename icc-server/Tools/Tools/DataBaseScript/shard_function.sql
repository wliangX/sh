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
				  create index '||index_name||'_'||min_time_begin_short_str||'_glzjjdbh on '||final_name||'_'||min_time_begin_short_str||' USING btree (glzjjdbh) WHERE glzjjdbh IS NOT NULL;
				  create index '||index_name||'_'||min_time_begin_short_str||'_crttime on '||final_name||'_'||min_time_begin_short_str||' (cjsj);';
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
				  CREATE INDEX idx_t_client_'||min_time_begin_short_str||'_hangup_type ON '||final_name||'_'||min_time_begin_short_str||' USING btree (hangup_type);';		 
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

select create_shardtable_icc_t_fkdb() as icc_t_fkdb, create_shardtable_icc_t_jjdb() as icc_t_jjdb, create_shardtable_icc_t_pjdb() as icc_t_pjdb, create_shardtable_icc_t_alarm_log() as icc_t_alarm_log, create_shardtable_icc_t_sub_callevent() as icc_t_sub_callevent, create_shardtable_icc_t_callevent() as icc_t_callevent;
