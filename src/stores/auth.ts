// src/stores/auth.ts
// 认证状态管理

import { defineStore } from 'pinia';
import { ref, computed } from 'vue';
import { invoke } from '@tauri-apps/api/core';
import type { Credentials } from '@/types';

export const useAuthStore = defineStore('auth', () => {
  // ============ State ============
  const userId = ref<string>('');
  const token = ref<string>('');
  const isValidating = ref<boolean>(false);
  const isValidated = ref<boolean>(false);
  const validationError = ref<string | null>(null);

  // ============ Getters ============
  const credentials = computed<Credentials>(() => ({
    user_id: userId.value,
    token: token.value,
  }));

  const hasCredentials = computed(() => {
    return userId.value.trim() !== '' && token.value.trim() !== '';
  });

  const isAuthenticated = computed(() => {
    return hasCredentials.value && isValidated.value;
  });

  // ============ Actions ============

  /**
   * 设置认证信息
   */
  function setCredentials(newUserId: string, newToken: string) {
    userId.value = newUserId;
    token.value = newToken;
    isValidated.value = false;
    validationError.value = null;
  }

  /**
   * 清除认证信息
   */
  function clearCredentials() {
    userId.value = '';
    token.value = '';
    isValidated.value = false;
    validationError.value = null;
  }

  /**
   * 从文件加载认证信息
   */
  async function loadFromFile(filePath: string): Promise<void> {
    try {
      const creds = await invoke<Credentials>('read_credentials_file', {
        filePath,
      });
      setCredentials(creds.user_id, creds.token);
    } catch (error) {
      throw new Error(`加载认证文件失败: ${error}`);
    }
  }

  /**
   * 保存认证信息到文件
   */
  async function saveToFile(filePath: string): Promise<void> {
    if (!hasCredentials.value) {
      throw new Error('没有有效的认证信息可保存');
    }
    try {
      await invoke('save_credentials_file', {
        filePath,
        userId: userId.value,
        token: token.value,
      });
    } catch (error) {
      throw new Error(`保存认证文件失败: ${error}`);
    }
  }

  /**
   * 验证认证信息
   */
  async function validateCredentials(): Promise<boolean> {
    if (!hasCredentials.value) {
      validationError.value = '请先填写用户 ID 和 Token';
      return false;
    }

    isValidating.value = true;
    validationError.value = null;

    try {
      const isValid = await invoke<boolean>('validate_credentials', {
        userId: userId.value,
        token: token.value,
      });

      isValidated.value = isValid;

      if (!isValid) {
        validationError.value = '认证信息无效，请检查用户 ID 和 Token';
      }

      return isValid;
    } catch (error) {
      validationError.value = `验证失败: ${error}`;
      isValidated.value = false;
      return false;
    } finally {
      isValidating.value = false;
    }
  }

  /**
   * 从 localStorage 恢复状态
   */
  function restoreFromStorage() {
    const savedUserId = localStorage.getItem('bangumi_user_id');
    const savedToken = localStorage.getItem('bangumi_token');
    if (savedUserId && savedToken) {
      userId.value = savedUserId;
      token.value = savedToken;
    }
  }

  /**
   * 保存状态到 localStorage
   */
  function saveToStorage() {
    if (hasCredentials.value) {
      localStorage.setItem('bangumi_user_id', userId.value);
      localStorage.setItem('bangumi_token', token.value);
    }
  }

  /**
   * 清除 localStorage 中的状态
   */
  function clearStorage() {
    localStorage.removeItem('bangumi_user_id');
    localStorage.removeItem('bangumi_token');
  }

  return {
    // State
    userId,
    token,
    isValidating,
    isValidated,
    validationError,
    // Getters
    credentials,
    hasCredentials,
    isAuthenticated,
    // Actions
    setCredentials,
    clearCredentials,
    loadFromFile,
    saveToFile,
    validateCredentials,
    restoreFromStorage,
    saveToStorage,
    clearStorage,
  };
});
