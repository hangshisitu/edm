package cn.edm.security;

import java.util.Set;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;

import com.google.common.collect.Sets;

import cn.edm.constant.ErrorCode;
import cn.edm.constant.Role;
import cn.edm.constant.Status;
import cn.edm.domain.Corp;
import cn.edm.domain.User;
import cn.edm.exception.UserNameOrPasswordException;
import cn.edm.exception.UserRoleException;
import cn.edm.exception.UserStatusException;
import cn.edm.service.CorpService;
import cn.edm.service.UserService;

/**
 * 对用户验证和授权
 * @author Luxh
 *
 */
public class UserDetailsServiceImpl implements UserDetailsService{
	
	@Autowired
	private UserService userService;
	@Autowired
	private CorpService corpService;
	
	@Override
	public UserDetails loadUserByUsername(String username)
			throws UsernameNotFoundException {
			User user = userService.getByUserId(username);
			//异常处理
			if(user == null) {
				throw new UserNameOrPasswordException(ErrorCode.USERNAME_OR_PASSWORD_NOT_CORRECT_MESSAGE);
			}else if(user.getStatus()!=Status.VALID) {
				throw new UserStatusException(ErrorCode.USER_STATUS_INVALID__MESSAGE);
			}else if(user.getRoleId()!=Role.ADM.getId()&&user.getRoleId()!=Role.SEE.getId()){
				throw new UserRoleException(ErrorCode.NOT_ADMIN_ROLE_MESSAGE);
			}else {
				Corp corp = corpService.getByCorpId(user.getCorpId());
				Set<GrantedAuthority> grantedAuths = obtainGrantedAuthorities(user);
				boolean enabled = true;
				boolean accountNonExpired = true;
				boolean credentialsNonExpired = true;
				boolean accountNonLocked = true;
				
				UserDetail userDetail = new UserDetail(user.getUserId(), user.getPassword(), 
						enabled, accountNonExpired, credentialsNonExpired, accountNonLocked, grantedAuths);
				userDetail.setCorpId(corp.getCorpId());
				userDetail.setCosId(corp.getCosId());
				
				return userDetail;
			}
			
		
		
	}
	
	/**
	 * 获取用户的角色权限
	 * @param user
	 * @return
	 */
	private Set<GrantedAuthority> obtainGrantedAuthorities(User user) {
		Set<GrantedAuthority> authSet = Sets.newHashSet();
		authSet.add(new SimpleGrantedAuthority(User.AUTHORITY_PREFIX+Role.getName(user.getRoleId())));
		return authSet;
	}

}
