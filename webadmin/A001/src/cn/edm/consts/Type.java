package cn.edm.consts;

/**
 * 任务类型.
 * 
 * @author yjli
 */
public class Type {

    public static final int FORMAL = 0;     // 单项任务
    public static final int TEST = 1;       // 测试任务
    public static final int PLAN = 2;       // 计划任务 (周期任务)
    public static final int COLL = 3;       // 计划明细 (周期任务的子任务)
    public static final int JOIN = 4;       // API对接任务
}
