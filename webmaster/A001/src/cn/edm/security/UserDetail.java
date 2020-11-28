package cn.edm.security;

import java.util.Collection;

import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.userdetails.User;

/**
 * 重新spring-security User
 * @author Luxh
 *
 */
public class UserDetail extends User{
	
	private static final long serialVersionUID = 3733056384459801923L;
	/**
	 * 套餐id
	 */
	private int cosId;
	
	/**
	 * 套餐企业id
	 */
	private int corpId;
	
	
	public UserDetail(String username, String password, boolean enabled,
			boolean accountNonExpired, boolean credentialsNonExpired,
			boolean accountNonLocked,
			Collection<? extends GrantedAuthority> authorities) {
		super(username, password, enabled, accountNonExpired, credentialsNonExpired,
				accountNonLocked, authorities);
	}

	

	public int getCosId() {
		return cosId;
	}

	public void setCosId(int cosId) {
		this.cosId = cosId;
	}

	public int getCorpId() {
		return corpId;
	}

	public void setCorpId(int corpId) {
		this.corpId = corpId;
	}

	

}
