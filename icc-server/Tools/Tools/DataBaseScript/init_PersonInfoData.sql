--	手工执行 init_PersonInfoData.sql

-- icc_t_district_code_map 为 全国行政区划 与 code

-- icc_t_houheadrel 为 户主关系表

-- icc_t_carnotype 为 汽车号牌关系表


DROP TABLE IF EXISTS "public"."icc_t_district_code_map";;;

CREATE TABLE icc_t_district_code_map (
  district_code character varying(30),
  district character varying(64)
);;;
COMMENT ON COLUMN icc_t_district_code_map.district_code IS '行政区划字典编码';;;
COMMENT ON COLUMN icc_t_district_code_map.district IS '行政区划名称';;;


INSERT INTO public.icc_t_district_code_map ("district_code", "district") VALUES ('152628', '内蒙古自治区丰镇县');;;


DROP TABLE IF EXISTS "public"."icc_t_houheadrel";;;
CREATE TABLE public.icc_t_houheadrel
(
    code character varying(64),
    relationship character varying(64)
);;;


INSERT INTO public.icc_t_houheadrel VALUES('100', '亲属关系');;;
INSERT INTO public.icc_t_houheadrel VALUES('157', '继父或养父');;;
INSERT INTO public.icc_t_houheadrel VALUES('202', '狱友');;;
INSERT INTO public.icc_t_houheadrel VALUES('323', '堂姐');;;
INSERT INTO public.icc_t_houheadrel VALUES('203', '同伙');;;
INSERT INTO public.icc_t_houheadrel VALUES('331', '夫妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('222', '社团成员');;;
INSERT INTO public.icc_t_houheadrel VALUES('102', '户主');;;
INSERT INTO public.icc_t_houheadrel VALUES('223', '商业交往');;;
INSERT INTO public.icc_t_houheadrel VALUES('103', '小集体户主');;;
INSERT INTO public.icc_t_houheadrel VALUES('101', '本人');;;
INSERT INTO public.icc_t_houheadrel VALUES('110', '配偶');;;
INSERT INTO public.icc_t_houheadrel VALUES('111', '夫');;;
INSERT INTO public.icc_t_houheadrel VALUES('112', '妻');;;
INSERT INTO public.icc_t_houheadrel VALUES('120', '子');;;
INSERT INTO public.icc_t_houheadrel VALUES('121', '独生子');;;
INSERT INTO public.icc_t_houheadrel VALUES('122', '长子');;;
INSERT INTO public.icc_t_houheadrel VALUES('123', '次子');;;
INSERT INTO public.icc_t_houheadrel VALUES('124', '三子');;;
INSERT INTO public.icc_t_houheadrel VALUES('125', '四子');;;
INSERT INTO public.icc_t_houheadrel VALUES('126', '五子');;;
INSERT INTO public.icc_t_houheadrel VALUES('127', '养子或继子');;;
INSERT INTO public.icc_t_houheadrel VALUES('128', '女婿');;;
INSERT INTO public.icc_t_houheadrel VALUES('129', '其他儿子');;;
INSERT INTO public.icc_t_houheadrel VALUES('130', '女');;;
INSERT INTO public.icc_t_houheadrel VALUES('131', '独生女');;;
INSERT INTO public.icc_t_houheadrel VALUES('132', '长女');;;
INSERT INTO public.icc_t_houheadrel VALUES('133', '次女');;;
INSERT INTO public.icc_t_houheadrel VALUES('134', '三女');;;
INSERT INTO public.icc_t_houheadrel VALUES('135', '四女');;;
INSERT INTO public.icc_t_houheadrel VALUES('136', '五女');;;
INSERT INTO public.icc_t_houheadrel VALUES('137', '养女或继女');;;
INSERT INTO public.icc_t_houheadrel VALUES('138', '儿媳');;;
INSERT INTO public.icc_t_houheadrel VALUES('139', '其他女儿');;;
INSERT INTO public.icc_t_houheadrel VALUES('140', '孙子、孙女或外孙子、外孙女');;;
INSERT INTO public.icc_t_houheadrel VALUES('141', '孙子');;;
INSERT INTO public.icc_t_houheadrel VALUES('142', '孙女');;;
INSERT INTO public.icc_t_houheadrel VALUES('143', '外孙子');;;
INSERT INTO public.icc_t_houheadrel VALUES('144', '外孙女');;;
INSERT INTO public.icc_t_houheadrel VALUES('145', '孙媳妇或外孙媳妇');;;
INSERT INTO public.icc_t_houheadrel VALUES('146', '孙女婿或外孙女婿');;;
INSERT INTO public.icc_t_houheadrel VALUES('147', '曾孙子或外曾孙子');;;
INSERT INTO public.icc_t_houheadrel VALUES('148', '曾孙女或外曾孙女');;;
INSERT INTO public.icc_t_houheadrel VALUES('149', '其他孙子、孙女或外孙子、外孙女');;;
INSERT INTO public.icc_t_houheadrel VALUES('150', '父母');;;
INSERT INTO public.icc_t_houheadrel VALUES('151', '父亲');;;
INSERT INTO public.icc_t_houheadrel VALUES('152', '母亲');;;
INSERT INTO public.icc_t_houheadrel VALUES('153', '公公');;;
INSERT INTO public.icc_t_houheadrel VALUES('154', '婆婆');;;
INSERT INTO public.icc_t_houheadrel VALUES('155', '岳父');;;
INSERT INTO public.icc_t_houheadrel VALUES('156', '岳母');;;
INSERT INTO public.icc_t_houheadrel VALUES('158', '继母或养母');;;
INSERT INTO public.icc_t_houheadrel VALUES('159', '其他父母关系');;;
INSERT INTO public.icc_t_houheadrel VALUES('160', '祖父母或外祖父母');;;
INSERT INTO public.icc_t_houheadrel VALUES('161', '祖父');;;
INSERT INTO public.icc_t_houheadrel VALUES('162', '祖母');;;
INSERT INTO public.icc_t_houheadrel VALUES('163', '外祖父');;;
INSERT INTO public.icc_t_houheadrel VALUES('164', '外祖母');;;
INSERT INTO public.icc_t_houheadrel VALUES('165', '配偶的祖父母或外祖父母');;;
INSERT INTO public.icc_t_houheadrel VALUES('166', '曾祖父');;;
INSERT INTO public.icc_t_houheadrel VALUES('167', '曾祖母');;;
INSERT INTO public.icc_t_houheadrel VALUES('168', '配偶的曾祖父母或外曾祖父母');;;
INSERT INTO public.icc_t_houheadrel VALUES('169', '其他祖父母或外祖父母关系');;;
INSERT INTO public.icc_t_houheadrel VALUES('170', '兄弟姐妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('171', '兄');;;
INSERT INTO public.icc_t_houheadrel VALUES('172', '嫂');;;
INSERT INTO public.icc_t_houheadrel VALUES('173', '弟');;;
INSERT INTO public.icc_t_houheadrel VALUES('174', '弟媳');;;
INSERT INTO public.icc_t_houheadrel VALUES('175', '姐姐');;;
INSERT INTO public.icc_t_houheadrel VALUES('176', '姐夫');;;
INSERT INTO public.icc_t_houheadrel VALUES('177', '妹妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('178', '妹夫');;;
INSERT INTO public.icc_t_houheadrel VALUES('179', '其他兄弟姐妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('181', '伯父');;;
INSERT INTO public.icc_t_houheadrel VALUES('182', '伯母');;;
INSERT INTO public.icc_t_houheadrel VALUES('183', '叔父');;;
INSERT INTO public.icc_t_houheadrel VALUES('184', '婶母');;;
INSERT INTO public.icc_t_houheadrel VALUES('185', '舅父');;;
INSERT INTO public.icc_t_houheadrel VALUES('186', '舅母');;;
INSERT INTO public.icc_t_houheadrel VALUES('187', '姨夫');;;
INSERT INTO public.icc_t_houheadrel VALUES('188', '姨母');;;
INSERT INTO public.icc_t_houheadrel VALUES('189', '姑父');;;
INSERT INTO public.icc_t_houheadrel VALUES('190', '姑母');;;
INSERT INTO public.icc_t_houheadrel VALUES('191', '堂兄弟、堂姐妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('192', '表兄弟、表姐妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('193', '侄子');;;
INSERT INTO public.icc_t_houheadrel VALUES('194', '侄女');;;
INSERT INTO public.icc_t_houheadrel VALUES('195', '外甥');;;
INSERT INTO public.icc_t_houheadrel VALUES('196', '外甥女');;;
INSERT INTO public.icc_t_houheadrel VALUES('197', '其他亲属');;;
INSERT INTO public.icc_t_houheadrel VALUES('198', '保姆');;;
INSERT INTO public.icc_t_houheadrel VALUES('199', '非亲属');;;
INSERT INTO public.icc_t_houheadrel VALUES('200', '社会交往关系');;;
INSERT INTO public.icc_t_houheadrel VALUES('201', '同案');;;
INSERT INTO public.icc_t_houheadrel VALUES('204', '赌友');;;
INSERT INTO public.icc_t_houheadrel VALUES('205', '帮派成员');;;
INSERT INTO public.icc_t_houheadrel VALUES('206', '同户');;;
INSERT INTO public.icc_t_houheadrel VALUES('207', '同住');;;
INSERT INTO public.icc_t_houheadrel VALUES('208', '同行');;;
INSERT INTO public.icc_t_houheadrel VALUES('209', '同乡');;;
INSERT INTO public.icc_t_houheadrel VALUES('210', '同事');;;
INSERT INTO public.icc_t_houheadrel VALUES('211', '情人');;;
INSERT INTO public.icc_t_houheadrel VALUES('212', '离异夫妻');;;
INSERT INTO public.icc_t_houheadrel VALUES('213', '结拜兄弟(姐妹)');;;
INSERT INTO public.icc_t_houheadrel VALUES('214', '校友');;;
INSERT INTO public.icc_t_houheadrel VALUES('215', '战友');;;
INSERT INTO public.icc_t_houheadrel VALUES('216', '师徒');;;
INSERT INTO public.icc_t_houheadrel VALUES('217', '教友');;;
INSERT INTO public.icc_t_houheadrel VALUES('218', '网友');;;
INSERT INTO public.icc_t_houheadrel VALUES('219', '邻里');;;
INSERT INTO public.icc_t_houheadrel VALUES('220', '监护关系');;;
INSERT INTO public.icc_t_houheadrel VALUES('221', '通讯联系关系');;;
INSERT INTO public.icc_t_houheadrel VALUES('224', '雇佣');;;
INSERT INTO public.icc_t_houheadrel VALUES('232', '同学');;;
INSERT INTO public.icc_t_houheadrel VALUES('235', '学友');;;
INSERT INTO public.icc_t_houheadrel VALUES('236', '朋友');;;
INSERT INTO public.icc_t_houheadrel VALUES('240', '师生');;;
INSERT INTO public.icc_t_houheadrel VALUES('261', '合伙人');;;
INSERT INTO public.icc_t_houheadrel VALUES('262', '客户');;;
INSERT INTO public.icc_t_houheadrel VALUES('270', '恋人');;;
INSERT INTO public.icc_t_houheadrel VALUES('272', '同性恋');;;
INSERT INTO public.icc_t_houheadrel VALUES('273', '男朋友');;;
INSERT INTO public.icc_t_houheadrel VALUES('274', '女朋友');;;
INSERT INTO public.icc_t_houheadrel VALUES('275', '未婚夫');;;
INSERT INTO public.icc_t_houheadrel VALUES('276', '未婚妻');;;
INSERT INTO public.icc_t_houheadrel VALUES('280', '前夫妻');;;
INSERT INTO public.icc_t_houheadrel VALUES('281', '前夫');;;
INSERT INTO public.icc_t_houheadrel VALUES('282', '前妻');;;
INSERT INTO public.icc_t_houheadrel VALUES('290', '其他社会交往关系');;;
INSERT INTO public.icc_t_houheadrel VALUES('300', '孙媳妇');;;
INSERT INTO public.icc_t_houheadrel VALUES('301', '外孙媳妇');;;
INSERT INTO public.icc_t_houheadrel VALUES('302', '孙女婿');;;
INSERT INTO public.icc_t_houheadrel VALUES('303', '外孙女婿');;;
INSERT INTO public.icc_t_houheadrel VALUES('304', '曾孙子');;;
INSERT INTO public.icc_t_houheadrel VALUES('305', '曾外孙子');;;
INSERT INTO public.icc_t_houheadrel VALUES('306', '曾外孙女');;;
INSERT INTO public.icc_t_houheadrel VALUES('307', '曾孙女');;;
INSERT INTO public.icc_t_houheadrel VALUES('308', '配偶祖父');;;
INSERT INTO public.icc_t_houheadrel VALUES('309', '配偶祖母');;;
INSERT INTO public.icc_t_houheadrel VALUES('310', '配偶外祖父');;;
INSERT INTO public.icc_t_houheadrel VALUES('311', '配偶外祖母');;;
INSERT INTO public.icc_t_houheadrel VALUES('316', '堂兄弟');;;
INSERT INTO public.icc_t_houheadrel VALUES('317', '堂姐妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('318', '表兄弟');;;
INSERT INTO public.icc_t_houheadrel VALUES('319', '表姐妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('320', '外曾祖父');;;
INSERT INTO public.icc_t_houheadrel VALUES('321', '外曾祖母');;;
INSERT INTO public.icc_t_houheadrel VALUES('322', '堂兄');;;
INSERT INTO public.icc_t_houheadrel VALUES('324', '堂弟');;;
INSERT INTO public.icc_t_houheadrel VALUES('325', '堂妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('326', '表兄');;;
INSERT INTO public.icc_t_houheadrel VALUES('327', '表姐');;;
INSERT INTO public.icc_t_houheadrel VALUES('328', '表弟');;;
INSERT INTO public.icc_t_houheadrel VALUES('329', '表妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('330', '夫弟');;;
INSERT INTO public.icc_t_houheadrel VALUES('332', '夫兄');;;
INSERT INTO public.icc_t_houheadrel VALUES('333', '夫姐');;;
INSERT INTO public.icc_t_houheadrel VALUES('334', '妻弟');;;
INSERT INTO public.icc_t_houheadrel VALUES('336', '妻兄');;;
INSERT INTO public.icc_t_houheadrel VALUES('348', '堂兄妹或堂姐弟');;;
INSERT INTO public.icc_t_houheadrel VALUES('349', '表兄妹或表姐弟');;;
INSERT INTO public.icc_t_houheadrel VALUES('400', '寄住');;;
INSERT INTO public.icc_t_houheadrel VALUES('401', '同寄住');;;
INSERT INTO public.icc_t_houheadrel VALUES('402', '租住');;;
INSERT INTO public.icc_t_houheadrel VALUES('600', '亲属关系补充');;;
INSERT INTO public.icc_t_houheadrel VALUES('601', '本人');;;
INSERT INTO public.icc_t_houheadrel VALUES('602', '户主');;;
INSERT INTO public.icc_t_houheadrel VALUES('611', '妻姐');;;
INSERT INTO public.icc_t_houheadrel VALUES('612', '妻妹');;;
INSERT INTO public.icc_t_houheadrel VALUES('621', '侄媳');;;
INSERT INTO public.icc_t_houheadrel VALUES('622', '侄女婿');;;
INSERT INTO public.icc_t_houheadrel VALUES('700', '亲属双向关系补充');;;
INSERT INTO public.icc_t_houheadrel VALUES('701', '母子');;;
INSERT INTO public.icc_t_houheadrel VALUES('702', '母女');;;
INSERT INTO public.icc_t_houheadrel VALUES('703', '父子');;;
INSERT INTO public.icc_t_houheadrel VALUES('704', '父女');;;
INSERT INTO public.icc_t_houheadrel VALUES('705', '婆媳');;;
INSERT INTO public.icc_t_houheadrel VALUES('706', '公媳');;;
INSERT INTO public.icc_t_houheadrel VALUES('711', '祖孙');;;
INSERT INTO public.icc_t_houheadrel VALUES('721', '叔侄');;;
INSERT INTO public.icc_t_houheadrel VALUES('722', '姑甥');;;
INSERT INTO public.icc_t_houheadrel VALUES('723', '舅侄');;;
INSERT INTO public.icc_t_houheadrel VALUES('724', '姨甥');;;
INSERT INTO public.icc_t_houheadrel VALUES('999', '其他');;;
INSERT INTO public.icc_t_houheadrel VALUES('341', '侄女（相对于伯父伯母）');;;
INSERT INTO public.icc_t_houheadrel VALUES('342', '侄子（相对于姑母姑父）');;;
INSERT INTO public.icc_t_houheadrel VALUES('343', '侄女（相对于姑母姑父）');;;
INSERT INTO public.icc_t_houheadrel VALUES('344', '外甥（相对于舅父舅母）');;;
INSERT INTO public.icc_t_houheadrel VALUES('345', '外甥（相对于姨夫姨母）');;;
INSERT INTO public.icc_t_houheadrel VALUES('346', '外甥女（相对于舅父舅母）');;;
INSERT INTO public.icc_t_houheadrel VALUES('347', '外甥女（相对于姨夫姨母）');;;
INSERT INTO public.icc_t_houheadrel VALUES('338', '侄子（相对于伯父伯母）');;;
INSERT INTO public.icc_t_houheadrel VALUES('339', '侄子（相对于叔父婶母）');;;
INSERT INTO public.icc_t_houheadrel VALUES('340', '侄女（相对于叔父婶母）');;;


DROP TABLE IF EXISTS "public"."icc_t_carnotype";;;
CREATE TABLE public.icc_t_carnotype
(
    code character varying(64),
    cartype character varying(64)
);;;


INSERT INTO public.icc_t_carnotype VALUES ('01', '大型汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('02', '小型汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('03', '使馆汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('04', '领馆汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('05', '境外汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('06', '外籍汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('07', '普通摩托车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('08', '轻便摩托车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('09', '使馆摩托车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('10', '领馆摩托车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('11', '境外摩托车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('12', '外籍摩托车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('13', '低速车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('14', '拖拉机号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('15', '挂车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('16', '教练汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('17', '教练摩托车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('18', '临时入境汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('19', '临时入境摩托车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('20', '临时行驶车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('21', '警用汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('22', '警用摩托号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('23', '原农机号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('24', '香港入出境车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('25', '澳门入出境车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('26', '武警号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('27', '军队号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('28', '应急号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('29', '无号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('30', '假号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('31', '挪用号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('33', '大型新能源汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('34', '小型新能源汽车号牌');;;
INSERT INTO public.icc_t_carnotype VALUES ('99', '其他号牌');;;


--省厅-登录鉴权接口	http://ip:port/icc-gateway-service/icc/gateway/get_login_token
--省厅-实名人员信息查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_id_info
--省厅-交通违法记录查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_veh_illegal_info
--省厅-人员案件查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_name_case_data
--省厅-案件信息查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_case_info
--省厅-机动车信息查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_cred_info
--省厅-户籍关系查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_cert_info
--省厅-车牌案件查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_case_veh_info
--省厅-人员标签查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_person_tag_info
--省厅-标准地址库查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_addr_info
--省厅-实有单位查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_comp_info
--省厅-实有房屋查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_hous
--省厅-人员主题库查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_person_info
--省厅-涉藏群体查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_tibetan_related_groups_info
--省厅-吸毒人员查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_drug_info
--省厅-在逃人员查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_escape_info
--省厅-重点人员查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_person_key_info
--省厅-照片信息查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_person_photo_info


--省厅-人员案件与案件详情查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_name_case_data_and_case_info
--省厅-户籍关系与人员主题库查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_cert_info_and_person_info
--省厅-机动车信息与人员主题库查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_cred_info_and_person_info
--省厅-车牌案件与案件详情查询接口	http://ip:port/icc-gateway-service/icc/gateway/query_case_veh_info_and_case_info


DROP TABLE IF EXISTS "public"."icc_t_post_url_map";;;
CREATE TABLE public.icc_t_post_url_map (
  http_cmd character varying(64),
  http_url character varying(512),
  http_serviceid character varying(64)
);;;
COMMENT ON TABLE icc_t_post_url_map IS '大数据post请求接口cmd与url对照表';
COMMENT ON COLUMN icc_t_post_url_map.http_cmd IS '请求的cmd';;;
COMMENT ON COLUMN icc_t_post_url_map.http_url IS '请求大数据post的url';;;
COMMENT ON COLUMN icc_t_post_url_map.http_serviceid IS '请求大数据post的serviceid';;;

INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('get_login_token', '/restcloud/api/v1/token/create','0000000000000000');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_id_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00026', '6628124917760000');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_name_case_data', '/restcloud/fhopenapiplatform/S-510000000000-0100-00051', '9790371823353856');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_veh_illegal_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00045', '9790273664057344');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_case_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00044', '9790273391427584');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_cred_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00039', '9790245784518656');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_cert_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00052', '9790457701728256');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_case_veh_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00050', '9790367410946048');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_person_tag_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00128', '19637802295623680');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_addr_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00042', '9790272791642112');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_comp_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00040', '9790272233799680');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_hous', '/restcloud/fhopenapiplatform/S-510000000000-0100-00057', '9790459194900480');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_person_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00053', '9790458041466880');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_tibetan_related_groups_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00063', '9790545752752128');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_drug_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00066', '9790546549669888');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_escape_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00064', '9790545991827456');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_person_key_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00071', '9790972992946176');;;
INSERT INTO public.icc_t_post_url_map ("http_cmd", "http_url", "http_serviceid") VALUES ('query_person_photo_info', '/restcloud/fhopenapiplatform/S-510000000000-0100-00019', '4068888580980736');;;



INSERT INTO public.icc_t_param (name, value) VALUES ('carplate', '号牌');;;