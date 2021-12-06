#!/usr/bin/env python3
"""
================================================
@File    :   SerialClient.py
@Author  :   wu sheng bang
@Contact :   shengbangx.wu@intel.com
@Time    :   2021/3/11 14:29
@Software:   PyCharm
@Desc    :
================================================
"""

import argparse
import os
import sys
import time
from lib.IAHost import *
from lib.Logging import MyLogging
from lib.SerialClient import *
from lib.analyze import *
from lib.comm import *
from lib.frame import *
from lib.PDU_control import *
from utils import transfer_log
import atexit
from signal import signal, SIGTERM, SIGINT


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("ia_host_ip", type=str, help="must specify ia host ip")
    parser.add_argument("user", type=str, help="the user on ia host")
    parser.add_argument("password", type=str, help="the password of the user which on ia host")
    parser.add_argument("mode", type=str, help="bypass, advance, streaming")
    parser.add_argument("timeout", type=int, help="timeout specified by minutes")
    parser.add_argument("transfer_way", type=int, help="0 -> soft_net; 1 -> fileSync")
    parser.add_argument("nvr8_11", type=int, help="nvr8, nvr11")
    parser.add_argument("debug", type=int, help="debug or not")
    parser.add_argument("secure", type=int, help="secure boot or open boot")
    parser.add_argument("config", type=str, help="replace config ")
    parser.add_argument("--TO", help="delivery email", required=False)
    parser.add_argument("--CC", help="CC email", required=False)
    parser.add_argument("--silver_case", help="silver test casename", required=False)

    args = parser.parse_args()

    mode = args.mode
    nvr8_11 = args.nvr8_11
    ia_host_ip = args.ia_host_ip
    host_id = os.popen(f"grep {ia_host_ip} {os.getcwd()}/machines.txt | cut -d: -f1").readline().split('\n')[0]
    nuc_ip = os.popen(f"grep {ia_host_ip} {os.getcwd()}/machines.txt | cut -d: -f5").readline().split('\n')[0]
    user = args.user
    pwd = args.password
    timeout = args.timeout
    transfer_way = args.transfer_way
    debug = args.debug
    secure = args.secure
    config = args.config
    to = args.TO
    cc = args.CC

    if not os.path.exists(log_root_path):
        os.makedirs(log_root_path)
    # /var/tmp/super_star/run.log只能有一个
    run_log = log_root_path + "/run.log"
    if os.path.exists(run_log):
        os.remove(run_log)

    # 生成/var/tmp/super_star/run.log
    log = MyLogging(run_log)
    logger = log.logger

    start = time.time()
    run_cnt = 1

    # 如果连接IA HOST失败，则脚本退出，测试失败，不生成任何目录和日志文件；此时，必须手动启动IA HOST，并再次运行
    ia_host = IAHost(ia_host_ip, user, pwd, mode, logger, transfer_way, nvr8_11, host_id)

    bkc_version = ia_host.run_cmd_get_output("cat ~/super_star/bkcs/bkc_version").split("\n")[0]
    bkc_url = ia_host.run_cmd_get_output("cat ~/super_star/bkcs/bkc_url").split("\n")[0]

    if args.silver_case:
        dir_name = bkc_version + "_" + args.silver_case + "_" + host_id + "_" + time.strftime("%Y-%m-%d_%H-%M-%S")
        log_path_prefix = log_root_path + "/silver_" + bkc_version + "/" + dir_name + "/"
    else:
        dir_name = bkc_version + "_" + host_id + "_" + time.strftime("%Y-%m-%d_%H-%M-%S")
        log_path_prefix = log_root_path + bkc_version + "/" + dir_name + "/"
    log_run_count_path = log_path_prefix + "long_run_count.txt"
    ret = 0
    count_max = 3
    seq_dict = {1: "1st", 2: "2nd", 3: "3rd"}

    # capture CTRL-C
    # signal(SIGINT, stop_test);
    # # capture kill command
    # signal(SIGTERM, stop_test);

    while not end_test and run_cnt <= count_max:
        if args.silver_case:
            log_path_of_this_test = log_path_prefix
        else:
            log_path_of_this_test = log_path_prefix + "{}/".format(run_cnt)
        os.makedirs(log_path_of_this_test)  # 生成本次测试的日志目录
        stability_result = log_path_of_this_test + "stability_result.log"
        ia_host = IAHost(ia_host_ip, user, pwd, mode, logger, transfer_way, nvr8_11, host_id)  # 释放之前的对象
        if args.silver_case:
            ia_host.silver_case_name = args.silver_case

        this_test_begin = int(time.time())

        ret = do_test(ia_host, logger, timeout, log_path_of_this_test, transfer_way, nvr8_11, secure, config, to, cc)
        run_time_of_this_time = int(time.time() - this_test_begin)
        bat_result = log_path_of_this_test + "bat_result.log"
        result_analyze(ia_host, stability_result, log_path_of_this_test, ret, runtime=run_time_of_this_time // 60)
        # send bat result to email if email exist
        if to is not None and to != "":
            with open(stability_result, "r") as f:
                # folder_name = os.path.basename(os.path.dirname(os.path.dirname(stability_result)))
                # email_subject = "{}:Stability {}".format(folder_name, "Passed" if ret == TEST_OK else "Failed")
                # [BKC E2E VAL]20211117-1550_rbf65057: NVR8 Bypass 1st try passed
                bkc_rbf = os.path.basename(os.path.dirname(os.path.dirname(bat_result)))[:-20]
                nvrN = "NVR8" if nvr8_11 == 8 else "NVR11"
                status = "Passed" if ret == TEST_OK else "Failed"
                BAT_Stability = "BAT" if nvr8_11 == 8 or not os.path.isfile(bat_result) else "Stability"
                email_subject = "[BKC E2E VAL]{}: {} {} {} {} try {}".format(bkc_rbf, nvrN, mode.capitalize(),
                                                                             BAT_Stability,
                                                                             seq_dict[
                                                                                 run_cnt] if run_cnt in seq_dict else "{}th".format(
                                                                                 run_cnt), status)
                send_email(248, email_subject, "<br>{}".format(f.read().replace("\n", "<br>")), to, cc)

        record_run_cnt(log_run_count_path, run_cnt, run_time_of_this_time, ret)
        run_cnt += 1
        if nvr8_11 == 11 and not os.path.exists(bat_result):
            restart_ia_host(host_id, ia_host_ip, logger)
            continue
        if not args.silver_case and debug == 0 and ret != TEST_OK and ia_host_ip != nuc_ip:
            restart_ia_host(host_id, ia_host_ip, logger)
        else:
            break
        if nvr8_11 == 11 and int(time.time() - start) < timeout * 60:
            continue

    # share = IAHost("cpifs.intel.com", "share", "share", mode, logger)
    # share.upload_logs(log_path_prefix)
    # transfer log to gio log folder
    print("Start to transfer thb log to gio folder")
    transfer_log.main(log_path_prefix)

    if "release" in bkc_url or "daily-build" in bkc_url:
        save_log_to_server(log_path_prefix, mode, "mainline")
    else:
        save_log_to_server(log_path_prefix, mode, "preint")
    # to combine all test result here
    sys.exit(ret)


#@atexit.register
def stop_test():
    end_test=True   # defined in lib/frame.py
'''
    print('*************stop test**************')
    run_time_of_this_test = int(time.time() - this_test_begin) // 60
    bat_result1 = log_path_of_this_test + "bat_result.log"
    result_analyze(ia_host, stability_result, log_path_of_this_test, ret, runtime=run_time_of_this_test)
    if to is not None and to != "":
        print('*************send email**************')
        with open(stability_result, "r") as f:
            bkc_rbf1 = os.path.basename(os.path.dirname(os.path.dirname(bat_result1)))[:-20]
            nvr = "NVR8" if nvr8_11 == 8 else "NVR11"
            stat = "Passed" if ret == TEST_OK else "Failed"
            bats = "BAT" if nvr8_11 == 8 or not os.path.isfile(bat_result1) else "Stability"
            email_s = "[BKC E2E VAL]{}: {} {} {} {} try {}".format(
                bkc_rbf1, nvr, mode.capitalize(), bats, seq_dict[run_cnt]
                if run_cnt in seq_dict else "{}th".format(run_cnt), stat)
            send_email(248, email_s, "<br>{}".format(f.read().replace("\n", "<br>")), to, cc)
'''
