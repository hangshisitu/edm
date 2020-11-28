package cn.edm.constant;
/**
 * <p>任务类型常量</p>
 * type: 0/1/2/3/4  </br>
 * 1. 0为单项任务 </br>
 * 2. 1为测试任务 </br>
 * 3. 2为周期任务父任务（无投递效果的任务） </br>
 * 4. 3为周期任务子任务（有投递效果的任务） </br>
 * 5. 4为API任务
 * 
 * @Date 2014年7月10日 上午11:28:39
 * @author Lich
 *
 */
public class TaskTypeConstant {
	/**
	 * 单项任务：0
	 */
	public static final Integer TASK_SINGLE = 0;
	/**
	 * 测试任务：1
	 */
	public static final Integer TASK_TEST = 1;
	/**
	 * 周期任务父任务（无投递效果的任务）：2
	 */
	public static final Integer TASK_PARENT_PLAN = 2;
	/**
	 * 周期任务子任务（有投递效果的任务）：3
	 */
	public static final Integer TASK_SON_PLAN = 3;
	/**
	 * API群发任务:4
	 */
	public static final Integer TASK_API_GROUPS = 4;
	
}
