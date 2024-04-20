DROP FUNCTION IF exists isnumeric();
CREATE OR REPLACE FUNCTION isnumeric (txtStr VARCHAR) 
RETURNS BOOLEAN AS $$
BEGIN
RETURN txtStr ~ '^([0-9]+[.]?[0-9]*|[.][0-9]+)$' ;
END; 
$$ LANGUAGE 'plpgsql';

DROP FUNCTION IF exists upgrad_icc_t_callevent(table_name text);
CREATE OR REPLACE FUNCTION upgrad_icc_t_callevent(table_name text)
RETURNS text AS $sql_str$
DECLARE
	strSQL   text;
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
begin
	ALTER TABLE public.icc_t_callevent ADD incoming_len interval;
	ALTER TABLE public.icc_t_callevent ADD ring_len interval;
	ALTER TABLE public.icc_t_callevent ADD talk_len interval;
	ALTER TABLE public.icc_t_callevent ADD record_file_id text;
	COMMENT ON COLUMN "public"."icc_t_callevent"."record_file_id" IS '录音文件ID';

	for dt_data in execute 'select callref_id,incoming_time,ring_time,talk_time,hangup_time,ringback_time from '||table_name||' where isnumeric(callref_id)=true' loop 
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
			strSQL := 'update '||table_name||' set "incoming_len" = '''||dt_incomming_len||''' where "callref_id" = '''||dt_data_str||''' and "create_time" >= '''||dt_create_time_start||''' and "create_time" <= '''||dt_create_time_end||''';';
		    execute strSQL;
		end if;
		if dt_data."talk_time" is not null and dt_data."ring_time" is not null then
			dt_ring_len := dt_data."talk_time" - dt_data."ring_time";
			strSQL := 'update '||table_name||' set "ring_len" = '''||dt_ring_len||''' where "callref_id" = '''||dt_data_str||''' and "create_time" >= '''||dt_create_time_start||''' and "create_time" <= '''||dt_create_time_end||''';';
	   	 	execute strSQL;
		elseif dt_data."talk_time" is not null and dt_data."ringback_time" is not null then
			dt_ring_len = dt_data."talk_time" - dt_data."ringback_time";
			strSQL := 'update '||table_name||' set "ring_len" = '''||dt_ring_len||''' where "callref_id" = '''||dt_data_str||''' and "create_time" >= '''||dt_create_time_start||''' and "create_time" <= '''||dt_create_time_end||''';';
	    	execute strSQL;
		end if;
	
		if dt_data."hangup_time" is not null and dt_data."talk_time" is not null then
			dt_talk_len := dt_data."hangup_time" - dt_data."talk_time";
			strSQL := 'update '||table_name||' set "talk_len" = '''||dt_talk_len||''' where "callref_id" = '''||dt_data_str||''' and "create_time" >= '''||dt_create_time_start||''' and "create_time" <= '''||dt_create_time_end||''';';
	    	execute strSQL;
		end if;
	end loop;
	return strSQL;
end;
$sql_str$ LANGUAGE plpgsql;

select upgrad_icc_t_callevent('icc_t_callevent');