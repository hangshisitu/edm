package cn.edm.persistence;

import java.util.List;
import java.util.Map;

import cn.edm.domain.User;


public interface UserMapper {
	
	/**
	 * 根据用户id查询用户
	 * @param userId 
	 * @return
	 */
	User selectByUserId(String userId);
	
	/**
	 * 分页查询记录数据
	 * @param paramsMap
	 * @return
	 */
	List<User> selectUsers(Map<String, Object> paramsMap);
	
	/**
	 * 查询记录数
	 * @param paramsMap
	 * @return
	 */
	long selectUsersCount(Map<String, Object> paramsMap);
	
	/**
	 * 更新账号状态
	 * @param paramsMap
	 */
	void updateUserStatus(Map<String, Object> paramsMap);	
	
	/**
	 * 添加账号
	 * @param user
	 */
	void insertUser(User user);
	
	/**
	 * 更新
	 * @param user
	 */
	void updateUser(User user);
	
     /**
	 * 根据企业id和角色id查询
	 * @param paramsMap
	 * @return
	 */
	User selectByCorpIdAndRoleId(Map<String, Object> paramsMap);
	
	/**
	 * 分页查询记录数据
	 * @param paramsMap
	 * @return
	 */
	List<User> selectResourceList(Map<String, Object> paramsMap);
	
	/**
	 * 查询记录数
	 * @param paramsMap
	 * @return
	 */
	long selectResourceCount(Map<String, Object> paramsMap);
	
	/**
	 * 根据角色id获取用户
	 * @param roleId
	 * @return
	 */
	User selectByRoleId(Integer roleId);
	
	/**
	 * 更新用户状态
	 * @param paramsMap
	 */
	void updateStatusByUserId(Map<String, Object> paramsMap);
	
	/**
	 * 根据email查询
	 * @param email
	 * @return
	 */
	User selectByEmail(String email);
	
	List<User> selectByCorpId(Integer corpId);
	
	User selectByEmailAndUserId(Map<String, Object> paramMap);
	
	User selectByUserIdAndRoleId(Map<String, Object> paramMap);
	

	
	/***   删除账号     ***/
	void deleteByUserId(String userId);
	
	void deleteCampaignByUserId(String userId);
	
	void deleteCategoryByUserId(String userId);
	
	void deleteCopyRecipientTriggerByUserId(String userId);
	
	void deleteCopyTagTriggerByUserId(String userId);
	
	void deleteFileByUserId(String userId);
	
	void deleteFilterByUserId(String userId);
	
	void deleteFormByUserId(String userId);
	
	void deleteLoginHistoryByUserId(String userId);
	
	void deletePlanByUserId(String userId);
	
	void deletePropByUserId(String userId);
	
	void deleteRecipientByUserId(String userId);
	
	void deleteSelectionByUserId(String userId);
	
	void deleteSenderByUserId(String userId);
	
	void deleteTagByUserId(String userId);
	
	void deleteTaskByUserId(String userId);
	
	void deleteTemplateByUserId(String userId);
	
	void deleteTemplateSettingByUserId(String userId);
	
	void deleteTouchByUserId(String userId);
	
	void deleteTrigerApiByUserId(String userId);

	/**
	 * 查询子账号
	 * @param UserId
	 * @return
	 */
	public List<User> getSubUserByManagerId(String managerId);
	/**
	 * 查询机构下面的账号
	 * @param corpId
	 * @return
	 */
	public List<User> selectAllUserByCorpId(String corpId);
	
	public Long selectUsersByCorpIdAndRoleId(Map<String, Object> paramMap);
	
	
}
