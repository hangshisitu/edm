package cn.edm.service;


import java.util.List;
import java.util.Map;

import cn.edm.domain.Corp;
import cn.edm.domain.User;
import cn.edm.util.Pagination;

/**
 * 用户业务接口
 * @author Luxh
 *
 */
public interface UserService {
	
	/**
	 * 根据用户id获取用户
	 * @param userId
	 * @return
	 */
	User getByUserId(String userId);
	
	/**
	 * 分页查询用户
	 * @param paramsMap		查询参数
	 * @param orderBy		排序
	 * @param currentPage	当前页
	 * @param pageSize		每页显示数
	 * @return
	 */
	Pagination<User> getPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	/**
	 * 保存账号
	 * @param corp
	 * @param user
	 */
	void save(User user);
	
	/**
	 * 保存企业
	 * @param corp
	 * @param user
	 */
	void save(Corp corp);
	
	/**
	 * 判断账号是否已存在
	 * @param userId
	 * @return
	 */
	boolean isUserExist(String userId);
	
	/**
	 * 更新企业
	 * @param corp
	 * @param user
	 */
	void update(Corp corp);
	
	/**
	 * 更新企业和账号
	 * @param corp
	 * @param user
	 */
	void update(Corp corp,User user);
	
	/**
	 * 更新账号
	 * @param corp
	 * @param user
	 */
	void update(User user);
	
	/**
	 * 分页查询渠道
	 * @param paramsMap
	 * @param orderBy
	 * @param currentPage
	 * @param pageSize
	 * @return
	 */
	Pagination<User> getResPagination(Map<String,Object> paramsMap,String orderBy,int currentPage,int pageSize);
	
	/**
	 * 根据角色id获取用户
	 * @param roleId
	 * @return
	 */
	User getByRoleId(Integer roleId);
	
	/**
	 * 保存用户
	 * @param user
	 */
	void saveUser(User user, boolean withRobots);
	
	/**
	 * 更新用户
	 * @param user
	 */
	void updateUser(User user, boolean withRobots);
	
	/**
	 * 删除账号,由该账号创建的所有数据将全部删除
	 * @param paramMap
	 */
	void deleteUser(String userId);
	
	/**
	 * 验证xss
	 * @param user
	 * @return
	 */
	Map<String,Object> verifyXss(User user);
	
	/**
	 * 验证xss
	 * @param user
	 * @return
	 */
	Map<String,Object> verifyXss(Corp corp);
	
	/**
	 * 判断邮箱是否已存在
	 * @param email
	 * @return
	 */
	boolean isEmailExist(String email);
	
	/**
	 * 验证账号字段否符合规则
	 * @param use
	 * @return
	 */
	Map<String,Object> verifyRegex(User use);

	
	/**
	 * 根据企业id和角色id获取用户
	 * @param paramMap
	 * @return
	 */
	User getByCorpIdAndRoleId(Map<String,Object> paramMap);
	
	Map<String,Object> verifySubUser(User user);
	
	Map<String,Object> verifyPassword(String password);
	
	List<User> getByCorpId(Integer corpId);
	
	User getByEmail(String email);
	
	boolean isEmailExistByUserId(Map<String,Object> paramMap);
	
	User getByUserIdAndRoleId(Map<String,Object> paramMap);
	
	void updateUserStatus(Map<String,Object> paramMap);
	
	void checkRobots(String[] robots);
	
	void isRobotsExist(String resId,String[] robots);
	
	void isOperateRobotsExist(String[] robots);
	
	List<User> getSubUserByManagerId(String UserId);
	
	public List<User> getAllUserByCorpId(String corpId);
	
	public Long selectUsersByCorpIdAndRoleId(Map<String,Object> paramMap);
	
}
