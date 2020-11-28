package cn.edm.constant;

import org.apache.commons.lang.StringUtils;

public class Status {
	
	/**
	 * 无效（禁用） -0
	 */
	public static final int INVALID = 0; 
	/**
	 * 有效（启用）- 1
	 */
	public static final int VALID = 1; 
	/**
	 * 冻结- 2
	 */
	public static final int FREEZE = 2; 
	
	/**
	 * 删除- 3
	 */
	public static final int DELETE = 3; 
	
	// ********** task status **********
	public static final int DRAFTED = 0; 		// 草稿
	
	public static final int PROOFING = 11; 		// 待批示
	public static final int UNAPPROVED = 12; 	// 批示不通过
	public static final int REVIEWING = 13; 	// 待审核
	public static final int RETURN = 14; 		// 审核不通过

	public static final int TEST = 21; 			// 测试发送
	public static final int WAITED = 22; 		// 待发送
	public static final int QUEUEING = 23; 		// 队列中
	public static final int PROCESSING = 24; 	// 发送中
	public static final int PAUSED = 25; 		// 已暂停
	public static final int CANCELLED = 26; 	// 已取消
	public static final int COMPLETED = 27; 	// 已完成
	
	// 任务优先级
	public static final int COMMON = 0;  // 优先
	public static final int PRIOR = 1;   // 普通
	
	/**
     * 获取任务状态集.
     * m:all|proofing|waited|processing|completed
     */
    public static Integer[] task(String m) {
        if (StringUtils.equals(m, "proofing")) {
            Integer[] statuses = { DRAFTED, PROOFING, UNAPPROVED, REVIEWING, RETURN };
            return statuses;
        } else if (StringUtils.equals(m, "waited")) {
            Integer[] statuses = { WAITED };
            return statuses;
        } else if (StringUtils.equals(m, "processing")) {
            Integer[] statuses = { QUEUEING, PROCESSING, PAUSED };
            return statuses;
        } else if (StringUtils.equals(m, "completed")) {
            Integer[] statuses = { CANCELLED, COMPLETED };
            return statuses;
        } else if (StringUtils.equals(m, "processing|completed")) {
            Integer[] statuses = { PROCESSING, PAUSED, CANCELLED, COMPLETED };
            return statuses;
        } else {
            Integer[] statuses = { DRAFTED, PROOFING, UNAPPROVED, REVIEWING, RETURN, WAITED, QUEUEING, PROCESSING, PAUSED, CANCELLED, COMPLETED };
            return statuses;
        }
    }
	
}
