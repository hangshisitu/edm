package cn.edm.security;

import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;

public class UserContext {
	
	public static UserDetail getCurrentUser() {
		Authentication auth = SecurityContextHolder.getContext().getAuthentication();
		return (UserDetail) auth.getPrincipal();
	}
}
