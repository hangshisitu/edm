package cn.edm.constants;


public class Status {

	public static final int DISABLED = 0;		// 无效/禁用
	public static final int ENABLED = 1;		// 有效/启用
	public static final int FREEZE = 2;			// 冻结
	public static final int DELETED = 3; 		// 删除
	
	// ********** task status **********
	public static final int DRAFTED = 0; 		// 草稿
	public static final int PLAN = 4; 			// 计划
	
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
}
