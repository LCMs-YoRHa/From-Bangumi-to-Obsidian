// src/types/index.ts
// TypeScript 类型定义 - 与 Rust models 对应

// ============ 认证相关 ============

export interface Credentials {
  user_id: string;
  token: string;
}

// ============ 条目相关 ============

export interface Subject {
  id: number;
  name: string;
  name_cn?: string;
  summary?: string;
  date?: string;
  platform?: string;
  total_episodes?: number;
  images?: SubjectImages;
  rating?: SubjectRating;
  tags: SubjectTag[];
  infobox: InfoboxItem[];
}

export interface SubjectImages {
  large?: string;
  common?: string;
  medium?: string;
  small?: string;
  grid?: string;
}

export interface SubjectRating {
  rank?: number;
  total?: number;
  score?: number;
}

export interface SubjectTag {
  name: string;
  count: number;
}

export interface InfoboxItem {
  key: string;
  value: unknown;
}

export interface SubjectPerson {
  id: number;
  name: string;
  relation?: string;
  images?: PersonImages;
}

export interface PersonImages {
  large?: string;
  medium?: string;
  small?: string;
  grid?: string;
}

export interface SubjectCharacter {
  id: number;
  name: string;
  relation?: string;
  images?: PersonImages;
  actors: Actor[];
}

export interface Actor {
  id: number;
  name: string;
  images?: PersonImages;
}

export interface SubjectRelation {
  id: number;
  name: string;
  name_cn?: string;
  relation?: string;
  subject_type?: number;
}

export interface SubjectFullData {
  subject: Subject;
  persons: SubjectPerson[];
  characters: SubjectCharacter[];
  relations: SubjectRelation[];
}

// ============ 收藏相关 ============

export enum SubjectType {
  Book = 1,
  Anime = 2,
  Music = 3,
  Game = 4,
  Real = 6,
}

export enum CollectionType {
  Wish = 1,      // 想看
  Collect = 2,   // 看过
  Doing = 3,     // 在看
  OnHold = 4,    // 搁置
  Dropped = 5,   // 抛弃
}

export const SubjectTypeLabels: Record<SubjectType, string> = {
  [SubjectType.Book]: '书籍',
  [SubjectType.Anime]: '动画',
  [SubjectType.Music]: '音乐',
  [SubjectType.Game]: '游戏',
  [SubjectType.Real]: '三次元',
};

export const CollectionTypeLabels: Record<CollectionType, string> = {
  [CollectionType.Wish]: '想看',
  [CollectionType.Collect]: '看过',
  [CollectionType.Doing]: '在看',
  [CollectionType.OnHold]: '搁置',
  [CollectionType.Dropped]: '抛弃',
};

export interface CollectionListResponse {
  total: number;
  limit: number;
  offset: number;
  data: CollectionItem[];
}

export interface CollectionItem {
  subject_id: number;
  subject_type: number;
  collection_type: number;
  rate?: number;
  comment?: string;
  tags: string[];
  is_private: boolean;
  updated_at?: string;
}

export interface UserCollection {
  subject_id: number;
  collection_type: number;
  rate?: number;
  comment?: string;
  tags: string[];
  is_private: boolean;
  updated_at?: string;
}

// ============ 批量导入参数 ============

export interface BatchImportParams {
  subject_type: number;
  collection_type?: number;
  limit: number;
  offset: number;
  auto_process: boolean;
}

export const defaultBatchImportParams: BatchImportParams = {
  subject_type: SubjectType.Anime,
  collection_type: undefined,
  limit: 30,
  offset: 0,
  auto_process: true,
};

// ============ 任务进度 ============

export interface TaskProgress {
  current: number;
  total: number;
  message: string;
  is_completed: boolean;
  is_error: boolean;
}

// ============ 文件相关 ============

export interface FileInfo {
  name: string;
  path: string;
  size: number;
  modified?: number;
}

// ============ 应用状态 ============

export type TaskStatus = 'idle' | 'running' | 'completed' | 'error';

export interface AppState {
  isLoading: boolean;
  error: string | null;
  taskStatus: TaskStatus;
  taskProgress: TaskProgress | null;
}
